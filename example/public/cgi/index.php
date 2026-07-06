<?php
function getPairedFilesInfo($directory)
{
    $directory = rtrim($directory, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR;
    if (!is_dir($directory)) {
        throw new InvalidArgumentException("Directory not found: $directory");
    }
    $files = scandir($directory);
    $result = [];
    foreach ($files as $file) {
        if (substr($file, -5) === '._txt') {
            $txtPath = $directory . $file;
            $baseName = substr($file, 0, -5);
            $pairedPath = $directory . $baseName;
            $content = file_get_contents($txtPath);
            if ($content === false) continue;
            $pairedExists = file_exists($pairedPath) && is_file($pairedPath);
            $result[] = [
                'txt_file'    => $txtPath,
                'content'     => trim($content),
                'paired_file' => $pairedExists ? $pairedPath : null,
                'paired_name' => $baseName,
            ];
        }
    }
    return $result;
}

function generateVideoThumbnail($videoPath, $thumbnailPath, $seekTime = '00:00:05')
{
    $videoEsc = escapeshellarg($videoPath);
    $thumbEsc = escapeshellarg($thumbnailPath);
    $cmd = "ffmpeg -ss $seekTime -i $videoEsc -vframes 1 -vf \"scale=320:-1\" -f webp $thumbEsc 2>&1";
    exec($cmd, $output, $returnCode);
    return ($returnCode === 0 && file_exists($thumbnailPath));
}

// ---------- CONFIG ----------
$folderPath = '/srv/upload';        // local filesystem path
$publicBase = '/upload';            // public URL prefix
$videoExts = ['mp4', 'mov', 'avi', 'mkv', 'webm', 'flv', 'm4v', '3gp', 'ogv'];
$imageExts = ['jpg', 'jpeg', 'png', 'gif', 'webp', 'svg', 'bmp', 'ico'];
// -----------------------------

try {
    $entries = getPairedFilesInfo($folderPath);
} catch (Exception $e) {
    die("Error: " . $e->getMessage());
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>File Gallery</title>
    <script src="https://cdn.tailwindcss.com"></script>
</head>

<body class="bg-gray-50 p-6">

    <div class="max-w-7xl mx-auto">
        <!-- Header -->
        <div class="flex items-center justify-between mb-6">
            <h1 class="text-3xl font-bold text-gray-800">📁 File Gallery</h1>
            <a href="/up.html" class="inline-flex items-center px-5 py-2.5 bg-blue-600 hover:bg-blue-700 text-white font-medium rounded-lg transition-colors shadow-sm">
                <svg class="w-5 h-5 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 4v16m8-8H4" />
                </svg>
                Upload
            </a>
        </div>

        <?php if (empty($entries)): ?>
            <div class="bg-yellow-50 border-l-4 border-yellow-400 p-4 rounded">
                <p class="text-yellow-700">No <code>._txt</code> files found.</p>
            </div>
        <?php else: ?>
            <div class="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4 gap-6">
                <?php foreach ($entries as $entry):
                    $randomName = $entry['paired_name'];
                    $content    = $entry['content'];
                    $pairedFile = $entry['paired_file'];
                    $fileExists = ($pairedFile !== null);

                    $publicUrl = $publicBase . '/' . $randomName;
                    $fileExt = '';
                    $isImage = false;
                    $isVideo = false;

                    if ($fileExists) {
                        $fileExt = strtolower(pathinfo($pairedFile, PATHINFO_EXTENSION));
                        $isImage = in_array($fileExt, $imageExts);
                        $isVideo = in_array($fileExt, $videoExts);
                    }

                    // Thumbnail for videos
                    $thumbnailUrl = null;
                    $thumbnailLocal = null;
                    if ($isVideo && $fileExists) {
                        $thumbnailLocal = $folderPath . '/' . $randomName . '.webp';
                        $thumbnailUrl = $publicBase . '/' . $randomName . '.webp';
                        if (!file_exists($thumbnailLocal)) {
                            generateVideoThumbnail($pairedFile, $thumbnailLocal, '00:00:05');
                        }
                        if (!file_exists($thumbnailLocal)) {
                            $thumbnailUrl = null;
                        }
                    }

                    $clickable = $isVideo && $fileExists;
                    $cardClass = 'bg-white rounded-xl shadow-md overflow-hidden hover:shadow-xl transition-shadow duration-200 flex flex-col relative';
                    if ($clickable) {
                        $cardClass .= ' video-card cursor-pointer';
                    }
                ?>
                    <div class="<?= $cardClass ?>"
                        <?= $clickable ? 'data-video-url="' . htmlspecialchars($publicUrl) . '"' : '' ?>
                        <?= $clickable ? 'role="button" tabindex="0"' : '' ?>
                        data-name="<?= htmlspecialchars($randomName) ?>">

                        <!-- Delete button (top-right corner) -->
                        <button class="delete-btn absolute top-2 right-2 z-10 p-1.5 bg-red-500 hover:bg-red-600 text-white rounded-full shadow-md transition-colors w-8 h-8 flex items-center justify-center"
                            data-name="<?= htmlspecialchars($randomName) ?>"
                            title="Delete this file">
                            <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16" />
                            </svg>
                        </button>

                        <div class="h-48 bg-gray-100 flex items-center justify-center overflow-hidden relative">
                            <?php if ($fileExists && $isImage): ?>
                                <img src="<?= htmlspecialchars($publicUrl) ?>"
                                    alt="<?= htmlspecialchars($content ?: $randomName) ?>"
                                    class="w-full h-full object-cover" loading="lazy" />
                            <?php elseif ($fileExists && $isVideo && $thumbnailUrl): ?>
                                <img src="<?= htmlspecialchars($thumbnailUrl) ?>"
                                    alt="<?= htmlspecialchars($content ?: $randomName) ?>"
                                    class="w-full h-full object-cover" loading="lazy" />
                                <!-- Play icon overlay -->
                                <div class="absolute inset-0 flex items-center justify-center pointer-events-none">
                                    <div class="bg-black bg-opacity-40 rounded-full p-3">
                                        <svg class="w-12 h-12 text-white fill-current" viewBox="0 0 24 24">
                                            <path d="M8 5v14l11-7z" />
                                        </svg>
                                    </div>
                                </div>
                            <?php elseif ($fileExists && $isVideo): ?>
                                <div class="text-center text-gray-500">
                                    <div class="text-5xl mb-1">🎬</div>
                                    <div class="text-xs"><?= htmlspecialchars($fileExt) ?></div>
                                </div>
                            <?php elseif ($fileExists): ?>
                                <div class="text-center p-4">
                                    <div class="text-5xl mb-2">📄</div>
                                    <div class="text-sm text-gray-600 truncate max-w-full">
                                        <?= htmlspecialchars($fileExt ?: 'file') ?>
                                    </div>
                                    <a href="<?= htmlspecialchars($publicUrl) ?>" download
                                        class="mt-2 inline-block px-4 py-1 bg-blue-500 text-white text-xs rounded hover:bg-blue-600 transition">
                                        Download
                                    </a>
                                </div>
                            <?php else: ?>
                                <div class="text-center text-gray-400 p-4">
                                    <div class="text-5xl mb-2">⚠️</div>
                                    <div class="text-sm">Paired file missing</div>
                                </div>
                            <?php endif; ?>
                        </div>

                        <div class="p-4 flex-1 flex flex-col">
                            <div class="text-sm font-medium text-gray-700 truncate" title="<?= htmlspecialchars($randomName) ?>">
                                📎 <?= htmlspecialchars($randomName) ?>
                            </div>
                            <?php if (!empty($content)): ?>
                                <p class="text-sm text-gray-600 mt-1 line-clamp-3 flex-1">
                                    <?= nl2br(htmlspecialchars($content)) ?>
                                </p>
                            <?php else: ?>
                                <p class="text-sm text-gray-400 italic mt-1">(no description)</p>
                            <?php endif; ?>
                        </div>
                    </div>
                <?php endforeach; ?>
            </div>
            <p class="mt-6 text-sm text-gray-500">Total: <strong><?= count($entries) ?></strong> items</p>
        <?php endif; ?>
    </div>

    <!-- Video Modal (Tailwind only) -->
    <div id="videoModal" class="fixed inset-0 bg-black/75 z-50 hidden items-center justify-center p-4">
        <div class="relative bg-black rounded-xl max-w-[90vw] max-h-[90vh] overflow-hidden shadow-2xl">
            <button id="closeModalBtn" class="absolute top-2 right-3 w-10 h-10 flex items-center justify-center bg-black/50 text-white text-3xl rounded-full hover:bg-white/30 transition-colors z-10">
                &times;
            </button>
            <video id="modalVideo" class="max-w-full max-h-[85vh] w-auto h-auto m-auto" controls autoplay preload="metadata">
                <source id="modalVideoSource" src="" type="video/mp4">
                Your browser does not support the video tag.
            </video>
        </div>
    </div>

    <script>
        (function() {
            // ----- Video Modal -----
            const modal = document.getElementById('videoModal');
            const video = document.getElementById('modalVideo');
            const source = document.getElementById('modalVideoSource');
            const closeBtn = document.getElementById('closeModalBtn');

            function openModal(videoUrl) {
                source.src = videoUrl;
                video.load();
                modal.classList.remove('hidden');
                modal.classList.add('flex');
            }

            function closeModal() {
                modal.classList.add('hidden');
                modal.classList.remove('flex');
                video.pause();
                source.src = '';
                video.load();
            }

            document.querySelectorAll('.video-card').forEach(card => {
                card.addEventListener('click', function(e) {
                    if (e.target.closest('a') || e.target.closest('.delete-btn')) return;
                    const url = this.dataset.videoUrl;
                    if (url) openModal(url);
                });
                card.addEventListener('keydown', function(e) {
                    if (e.key === 'Enter' || e.key === ' ') {
                        e.preventDefault();
                        const url = this.dataset.videoUrl;
                        if (url) openModal(url);
                    }
                });
            });

            closeBtn.addEventListener('click', closeModal);
            modal.addEventListener('click', function(e) {
                if (e.target === modal) closeModal();
            });
            document.addEventListener('keydown', function(e) {
                if (e.key === 'Escape' && !modal.classList.contains('hidden')) {
                    closeModal();
                }
            });
            video.addEventListener('click', function(e) {
                e.stopPropagation();
            });

            // ----- Delete functionality -----
            document.querySelectorAll('.delete-btn').forEach(btn => {
                btn.addEventListener('click', function(e) {
                    e.stopPropagation(); // prevent opening video modal / card click
                    const name = this.dataset.name;
                    const displayName = name || 'this file';
                    if (!confirm(`Are you sure you want to delete "${displayName}"?`)) {
                        return;
                    }

                    // Send POST request to delete.php
                    const formData = new FormData();
                    formData.append('name', name);

                    fetch('/delete.php', {
                            method: 'POST',
                            body: formData
                        })
                        .then(response => response.json())
                        .then(data => {
                            if (data.success) {
                                // Remove the card from DOM
                                const card = this.closest('.bg-white.rounded-xl');
                                if (card) {
                                    card.remove();
                                }
                                // Optionally update the total count
                                const totalEl = document.querySelector('p.mt-6 strong');
                                if (totalEl) {
                                    const current = parseInt(totalEl.textContent);
                                    totalEl.textContent = current - 1;
                                }
                            } else {
                                alert('Error: ' + data.message);
                            }
                        })
                        .catch(error => {
                            alert('Network error: ' + error.message);
                        });
                });
            });
        })();
    </script>

</body>

</html>