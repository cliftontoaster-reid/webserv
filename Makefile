# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jfranc <jfranc@student.42nice.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/19 11:27:55 by jfranc            #+#    #+#              #
#    Updated: 2026/05/19 11:27:58 by jfranc           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CCX = clang++

CCX_FLAGS ?= -Wall -Wextra -Werror -std=c++98 -pipe -MMD -MP
LDX_FLAGS ?= 

DEBUG_CFLAGS	 = -g3 -O0 -fno-omit-frame-pointer -D_GLIBCXX_DEBUG \
								 -fsanitize=address,undefined -fstack-protector-strong
DEBUG_LDFLAGS	= -fsanitize=address,undefined

COVERAGE_CFLAGS  = -O0 -g -fprofile-instr-generate -fcoverage-mapping
COVERAGE_LDFLAGS = -fprofile-instr-generate

RELEASE_CFLAGS = -O3 -DNDEBUG -D_FORTIFY_SOURCE=2 \
								 -fstack-protector-strong -ffunction-sections -fdata-sections
RELEASE_LDFLAGS = -Wl,--gc-sections -Wl,-z,relro -Wl,-z,now

ifeq ($(shell which clang++),)
	CCX = g++
endif
ifneq ($(shell which mold),)
	LDX_FLAGS += -fuse-ld=mold
endif
ifeq ($(shell tput colors 2>/dev/null),256)
	RESET = \033[0m
	BOLD = \033[1m
	RED = \033[31m
	GREEN = \033[32m
	YELLOW = \033[33m
	BLUE = \033[34m
else
	RESET =
	BOLD =
	RED =
	GREEN =
	YELLOW =
	BLUE =
endif

MODE ?= debug
CFLAGS =
ifeq ($(MODE),release)
	CFLAGS	= $(RELEASE_CFLAGS)
	LDFLAGS = $(RELEASE_LDFLAGS)
else ifeq ($(MODE),debug)
	CFLAGS	= $(DEBUG_CFLAGS)
	LDFLAGS = $(DEBUG_LDFLAGS)
else ifeq ($(MODE),coverage)
	CFLAGS	= $(COVERAGE_CFLAGS)
	LDFLAGS = $(COVERAGE_LDFLAGS)
else
	$(error "Invalid MODE specified: $(MODE). Use 'debug' or 'release'.")
endif

# -------
# Project
# -------
NAME = webserv
all: $(NAME)

LIBS_DIR	 =	./libs
ORIGIN_DIR =	./target
OFFICE_DIR =	$(ORIGIN_DIR)/$(MODE)
TARGET_DIR ?= $(OFFICE_DIR)/$(NAME)
OBJ_DIR		 =	$(TARGET_DIR)/obj
TOBJ_DIR	 =	$(TARGET_DIR)/tobj
DEP_DIR		 =	$(TARGET_DIR)/dep
BIN_DIR		 =	$(TARGET_DIR)/bin

INC_DIR		= ./include
SRC_DIR		= ./src
TEST_DIR	= ./tests

# -------
# Sources
# -------
SRCS = \

OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEP := $(patsubst $(SRC_DIR)/%.c,$(DEP_DIR)/%.d,$(SRCS))
INC := -I$(INC_DIR)
DIRS = $(OBJ_DIR) $(TOBJ_DIR) $(DEP_DIR) $(BIN_DIR)

dirs:
	@$(foreach d, $(DIRS), mkdir -p "$(d)";)

LIBRARIES := toml98 mon-cgi mon-http mon-net mon-router
LIBS_ARCHIVES :=

# -------
# TOML98
# -------
TOML98_DEPO			  = $(LIBS_DIR)/toml98
TOML98_TARGET_DIR = $(OFFICE_DIR)/toml98
TOML98_OBJ_DIR		= $(TOML98_TARGET_DIR)/obj
TOML98_TOBJ_DIR	  = $(TOML98_TARGET_DIR)/tobj
TOML98_DEP_DIR		= $(TOML98_TARGET_DIR)/dep
TOML98_BIN_DIR		= $(TOML98_TARGET_DIR)/bin
TOML98_DIRS			  = $(TOML98_OBJ_DIR) $(TOML98_TOBJ_DIR) \
										$(TOML98_DEP_DIR) $(TOML98_BIN_DIR)
TOML98_ARCHIVE		= $(TOML98_BIN_DIR)/libtoml98.a
TOML98_INC_DIR		= $(TOML98_DEPO)/include

LIBS_ARCHIVES += $(TOML98_ARCHIVE)
INC += -I$(TOML98_INC_DIR)

toml98: $(TOML98_ARCHIVE)
$(TOML98_ARCHIVE):
	@printf "$(BOLD)Building toml98 library...$(RESET)\n"
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
	TARGET_DIR="$(abspath $(TOML98_TARGET_DIR))" MODE="$(MODE)" \
	$(MAKE) -C $(TOML98_DEPO) all
	@printf "$(BOLD)Built toml98:$(RESET) $(GREEN)$(TOML98_ARCHIVE)$(RESET)\n"

# --------
# MON-CGI
# --------
MON_CGI_DEPO			 = $(LIBS_DIR)/mon-cgi
MON_CGI_TARGET_DIR = $(OFFICE_DIR)/mon-cgi
MON_CGI_OBJ_DIR		 = $(MON_CGI_TARGET_DIR)/obj
MON_CGI_TOBJ_DIR	 = $(MON_CGI_TARGET_DIR)/tobj
MON_CGI_DEP_DIR		 = $(MON_CGI_TARGET_DIR)/dep
MON_CGI_BIN_DIR		 = $(MON_CGI_TARGET_DIR)/bin
MON_CGI_DIRS			 = $(MON_CGI_OBJ_DIR) $(MON_CGI_TOBJ_DIR) \
										 $(MON_CGI_DEP_DIR) $(MON_CGI_BIN_DIR)
MON_CGI_ARCHIVE		= $(MON_CGI_BIN_DIR)/libmon_cgi.a
MON_CGI_INC_DIR		= $(MON_CGI_DEPO)/include

LIBS_ARCHIVES += $(MON_CGI_ARCHIVE)
INC += -I$(MON_CGI_INC_DIR)

mon-cgi: $(MON_CGI_ARCHIVE)
$(MON_CGI_ARCHIVE):
	@printf "$(BOLD)Building mon-cgi library...$(RESET)\n"
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
	TARGET_DIR="$(abspath $(MON_CGI_TARGET_DIR))" MODE="$(MODE)" \
	$(MAKE) -C $(MON_CGI_DEPO) all
	@printf "$(BOLD)Built mon-cgi:$(RESET) $(GREEN)$(MON_CGI_ARCHIVE)$(RESET)\n"

# ---------
# MON-HTTP
# ---------
MON_HTTP_DEPO			  = $(LIBS_DIR)/mon-http
MON_HTTP_TARGET_DIR = $(OFFICE_DIR)/mon-http
MON_HTTP_OBJ_DIR		= $(MON_HTTP_TARGET_DIR)/obj
MON_HTTP_TOBJ_DIR	  = $(MON_HTTP_TARGET_DIR)/tobj
MON_HTTP_DEP_DIR		= $(MON_HTTP_TARGET_DIR)/dep
MON_HTTP_BIN_DIR		= $(MON_HTTP_TARGET_DIR)/bin
MON_HTTP_DIRS			  = $(MON_HTTP_OBJ_DIR) $(MON_HTTP_TOBJ_DIR) \
											$(MON_HTTP_DEP_DIR) $(MON_HTTP_BIN_DIR)
MON_HTTP_ARCHIVE		= $(MON_HTTP_BIN_DIR)/libmon_http.a
MON_HTTP_INC_DIR		= $(MON_HTTP_DEPO)/include

LIBS_ARCHIVES += $(MON_HTTP_ARCHIVE)
INC += -I$(MON_HTTP_INC_DIR)

mon-http: $(MON_HTTP_ARCHIVE)
$(MON_HTTP_ARCHIVE):
	@printf "$(BOLD)Building mon-http library...$(RESET)\n"
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
	TARGET_DIR="$(abspath $(MON_HTTP_TARGET_DIR))" MODE="$(MODE)" \
	$(MAKE) -C $(MON_HTTP_DEPO) all
	@printf "$(BOLD)Built mon-http:$(RESET) $(GREEN)$(MON_HTTP_ARCHIVE)$(RESET)\n"

# -------
# MON-NET
# -------
MON_NET_DEPO			 = $(LIBS_DIR)/mon-net
MON_NET_TARGET_DIR = $(OFFICE_DIR)/mon-net
MON_NET_OBJ_DIR		 = $(MON_NET_TARGET_DIR)/obj
MON_NET_TOBJ_DIR	 = $(MON_NET_TARGET_DIR)/tobj
MON_NET_DEP_DIR		 = $(MON_NET_TARGET_DIR)/dep
MON_NET_BIN_DIR		 = $(MON_NET_TARGET_DIR)/bin
MON_NET_DIRS			 = $(MON_NET_OBJ_DIR) $(MON_NET_TOBJ_DIR) \
										 $(MON_NET_DEP_DIR) $(MON_NET_BIN_DIR)
MON_NET_ARCHIVE		= $(MON_NET_BIN_DIR)/libmon_net.a
MON_NET_INC_DIR		= $(MON_NET_DEPO)/include

LIBS_ARCHIVES += $(MON_NET_ARCHIVE)
INC += -I$(MON_NET_INC_DIR)

mon-net: $(MON_NET_ARCHIVE)
$(MON_NET_ARCHIVE):
	@printf "$(BOLD)Building mon-net library...$(RESET)\n"
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
	TARGET_DIR="$(abspath $(MON_NET_TARGET_DIR))" MODE="$(MODE)" \
	$(MAKE) -C $(MON_NET_DEPO) all
	@printf "$(BOLD)Built mon-net:$(RESET) $(GREEN)$(MON_NET_ARCHIVE)$(RESET)\n"

# -----------
# MON-ROUTER
# -----------
MON_ROUTER_DEPO			  = $(LIBS_DIR)/mon-router
MON_ROUTER_TARGET_DIR = $(OFFICE_DIR)/mon-router
MON_ROUTER_OBJ_DIR		= $(MON_ROUTER_TARGET_DIR)/obj
MON_ROUTER_TOBJ_DIR	  = $(MON_ROUTER_TARGET_DIR)/tobj
MON_ROUTER_DEP_DIR		= $(MON_ROUTER_TARGET_DIR)/dep
MON_ROUTER_BIN_DIR		= $(MON_ROUTER_TARGET_DIR)/bin
MON_ROUTER_DIRS			  = $(MON_ROUTER_OBJ_DIR) $(MON_ROUTER_TOBJ_DIR) \
												$(MON_ROUTER_DEP_DIR) $(MON_ROUTER_BIN_DIR)
MON_ROUTER_ARCHIVE		= $(MON_ROUTER_BIN_DIR)/libmon_router.a
MON_ROUTER_INC_DIR		= $(MON_ROUTER_DEPO)/include

LIBS_ARCHIVES += $(MON_ROUTER_ARCHIVE)
INC += -I$(MON_ROUTER_INC_DIR)

mon-router: $(MON_ROUTER_ARCHIVE)
$(MON_ROUTER_ARCHIVE):
	@printf "$(BOLD)Building mon-router library...$(RESET)\n"
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
	TARGET_DIR="$(abspath $(MON_ROUTER_TARGET_DIR))" MODE="$(MODE)" \
	$(MAKE) -C $(MON_ROUTER_DEPO) all
	@printf "$(BOLD)Built mon-router:$(RESET) $(GREEN)$(MON_ROUTER_ARCHIVE)$(RESET)\n"

# -----
# Tests
# -----
test:
	@printf "$(BOLD)Building tests...$(RESET)\n"
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LDX_FLAGS="$(LDX_FLAGS)" \
	TARGET_DIR="$(TARGET_DIR)" MODE="$(MODE)" \
	BOBJ="$(OBJ)" BINC="$(INC)" LIBS_ARCHIVES="$(LIBS_ARCHIVES)" \
	$(MAKE) -C $(TEST_DIR) all --silent
	@printf "$(BOLD)Built tests$(RESET)\n"

toml98-test:
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LDX_FLAGS="$(LDX_FLAGS)" \
	TARGET_DIR="$(abspath $(TARGET_DIR))/toml98" MODE="$(MODE)" \
	$(MAKE) -C $(LIBS_DIR)/toml98 test --silent

mon-cgi-test:
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LDX_FLAGS="$(LDX_FLAGS)" \
	TARGET_DIR="$(abspath $(TARGET_DIR))/mon-cgi" MODE="$(MODE)" \
	$(MAKE) -C $(LIBS_DIR)/mon-cgi test --silent

mon-http-test:
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LDX_FLAGS="$(LDX_FLAGS)" \
	TARGET_DIR="$(abspath $(TARGET_DIR))/mon-http" MODE="$(MODE)" \
	$(MAKE) -C $(LIBS_DIR)/mon-http test --silent

mon-net-test:
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LDX_FLAGS="$(LDX_FLAGS)" \
	TARGET_DIR="$(abspath $(TARGET_DIR))/mon-net" MODE="$(MODE)" \
	$(MAKE) -C $(LIBS_DIR)/mon-net test --silent

mon-router-test:
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LDX_FLAGS="$(LDX_FLAGS)" \
	TARGET_DIR="$(abspath $(TARGET_DIR))/mon-router" MODE="$(MODE)" \
	$(MAKE) -C $(LIBS_DIR)/mon-router test --silent

test-all: test toml98-test mon-cgi-test mon-http-test mon-net-test mon-router-test
	@printf "$(BOLD)$(GREEN)All tests built successfully!$(RESET)\n"

compile_commands.json: Makefile $(SRCS)
	@echo "Generating compile_commands.json ..."
	@python3 scripts/generate_compile_commands.py \
		--output $@ \
		--directory $(CURDIR) \
		--compiler $(CCX) \
		$(foreach f,$(CCX_FLAGS),--cflags=$(f)) \
		$(foreach f,$(CFLAGS),--cppflags=$(f)) \
		$(foreach i,$(INC),--inc=$(i)) \
		$(foreach s,$(SRCS),--src=$(s)) \
		--obj-dir $(OBJ_DIR) \
		--src-dir $(SRC_DIR) \
		--dep-dir $(DEP_DIR)
	@echo "compile_commands.json generated."

.linkflag_$(MODE):
	@$(RM) -f .linkflag_*
	@touch $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D) $(dir $(DEP_DIR)/$*.d)
	@$(CCX) $(CCX_FLAGS) $(CFLAGS) \
		-MF $(DEP_DIR)/$*.d -c $< -o $@ $(INC)
	@printf "$(BOLD)Compiled$(RESET) $(BLUE)$<$(RESET) -> $(GREEN)$@$(RESET) $(BOLD)$(RED)$(DEP_DIR)/$*.d$(RESET)\n"

$(BIN_DIR)/$(NAME): $(OBJ) $(LIBS_ARCHIVES)
	@$(CCX) -o "$@" $(LDX_FLAGS) $(LDFLAGS) \
		$(OBJ) $(LIBS_ARCHIVES)
	@printf "$(BOLD)Linked$(RESET) $(GREEN)$(NAME)$(RESET)$(BOLD)" at "$(RESET)$(GREEN)$@$(RESET)\n"

$(NAME): .linkflag_$(MODE) $(BIN_DIR)/$(NAME)
	@cp $(BIN_DIR)/$(NAME) ./$(NAME)
	@printf " $(BOLD)Copied$(RESET) $(GREEN)$(NAME)$(RESET) $(GREEN)to project root.$(RESET)\n"

format:
	@printf "$(BOLD)Formatting all source files...$(RESET)\n"
	@find . -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.tpp' \) -exec clang-format -i {} +; true

clang-tidy:
	@printf "$(BOLD)Running clang-tidy on all projects...$(RESET)\n"
	clang-tidy $(SRCS) -- $(CCX_FLAGS) $(INC)
	@for dir in $$(find . -name Makefile -not -path './Makefile' -exec dirname {} \; | sort -u); do \
		$(MAKE) -C "$$dir" clang-tidy --silent; \
	done

clean:
	@for lib in toml98 mon-cgi mon-http mon-net mon-router; do \
		TARGET_DIR="$(OFFICE_DIR)/$$lib" MODE="$(MODE)" \
		$(MAKE) -C $(LIBS_DIR)/$$lib clean --silent; \
	done
	@$(RM) -rfv "$(OBJ_DIR)" "$(TOBJ_DIR)"

fclean:
	@for lib in toml98 mon-cgi mon-http mon-net mon-router; do \
		TARGET_DIR="$(OFFICE_DIR)/$$lib" MODE="$(MODE)" \
		$(MAKE) -C $(LIBS_DIR)/$$lib fclean --silent; \
	done
	@$(RM) -rf "$(ORIGIN_DIR)"
	@$(RM) -f "./$(NAME)"
	@$(RM) -f .linkflag_*

re: fclean all

.PHONY: all clean fclean re dirs test toml98-test mon-cgi-test mon-http-test mon-net-test mon-router-test test-all format clang-tidy
-include $(DEP)
