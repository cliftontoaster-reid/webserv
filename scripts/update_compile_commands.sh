#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Directories that have a Makefile with a compile_commands.json target
MAKEFILE_DIRS=(
	"."
	"tests"
	"libs/toml98"
	"libs/toml98/tests"
	"libs/mon-cgi"
	"libs/mon-cgi/tests"
	"libs/mon-http"
	"libs/mon-http/tests"
	"libs/mon-net"
	"libs/mon-net/tests"
	"libs/mon-router"
	"libs/mon-router/tests"
)

# Source/include directories to watch (relative to PROJECT_ROOT)
WATCH_DIRS=()
for d in "${MAKEFILE_DIRS[@]}"; do
	WATCH_DIRS+=("$d/src" "$d/include")
done

run_for_dir() {
	local dir="$1"
	local make_dir="$PROJECT_ROOT/$dir"
	if [ ! -f "$make_dir/Makefile" ]; then
		return
	fi
	printf "\033[1mGenerating compile_commands.json in %s...\033[0m\n" "$dir"
	make -C "$make_dir" --no-print-directory compile_commands.json 2>&1 |
		while IFS= read -r line; do printf "  [%s] %s\n" "$dir" "$line"; done || true
}

map_file_to_dirs() {
	local file="$1"
	local rel="${file#$PROJECT_ROOT/}"
	local matched=()
	for dir in "${MAKEFILE_DIRS[@]}"; do
		if [[ "$rel" == "$dir"/* ]] || [ "$rel" = "$dir" ]; then
			matched+=("$dir")
		fi
	done
	if [ ${#matched[@]} -eq 0 ]; then
		echo "."
	else
		printf "%s\n" "${matched[@]}"
	fi
}

# Normalise path (readlink -f) for consistent comparison
resolve() {
	readlink -f "$1" 2>/dev/null || echo "$1"
}

if [ $# -eq 0 ]; then
	for dir in "${MAKEFILE_DIRS[@]}"; do
		run_for_dir "$dir"
	done
	exit 0
fi

case "$1" in
--watch)
	shift
	filter="${1:-.*\.(cpp|hpp|tpp)$}"
	printf "\033[1mWatching for file creation/deletion...\033[0m\n"
	# Build the list of absolute watch paths, keeping only those that exist
	watch_paths=()
	for d in "${WATCH_DIRS[@]}"; do
		abs="$PROJECT_ROOT/$d"
		[ -d "$abs" ] && watch_paths+=("$abs")
	done
	if [ ${#watch_paths[@]} -eq 0 ]; then
		echo "No source directories found to watch."
		exit 1
	fi
	# Track recent runs to avoid rapid duplicates
	last_run=""
	inotifywait -m -r -e create -e delete --format '%w%f' "${watch_paths[@]}" 2>/dev/null |
		while IFS= read -r file; do
			[[ "$file" =~ $filter ]] || continue
			now=$(date +%s)
			if [ "$now" = "$last_run" ]; then
				continue
			fi
			last_run="$now"
			dirs=$(map_file_to_dirs "$file")
			while IFS= read -r dir; do
				[ -n "$dir" ] && run_for_dir "$dir"
			done <<<"$dirs"
		done
	;;
--help|-h)
	echo "Usage: $0 [--watch] [<file>...]"
	echo ""
	echo "  (no args)      Regenerate compile_commands.json in all subprojects."
	echo "  <file> ...     Regenerate for the subproject(s) containing each file."
	echo "  --watch        Watch source directories for create/delete events and"
	echo "                 regenerate automatically."
	exit 0
	;;
*)
	# One or more file paths – regenerate matching subprojects
	for file in "$@"; do
		resolved="$(resolve "$file")"
		dirs=$(map_file_to_dirs "$resolved")
		while IFS= read -r dir; do
			[ -n "$dir" ] && run_for_dir "$dir"
		done <<<"$dirs"
	done
	;;
esac
