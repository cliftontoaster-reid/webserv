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

CCX_FLAGS ?= -Wall -Wextra -Werror -std=c++11 -pipe -MMD -MP
LDX_FLAGS ?= 

DEBUG_CFLAGS	 = -g3 -O0 -fno-omit-frame-pointer -D_GLIBCXX_DEBUG \
								 -fsanitize=address,undefined -fstack-protector-strong
DEBUG_LDFLAGS	= -fsanitize=address,undefined

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
else
	$(error "Invalid MODE specified: $(MODE). Use 'debug' or 'release'.")
endif

CFLAGS += -DMON_NET_TESTS

# -------
# Project
# -------
NAME = mon_net_test
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
	$(SRC_DIR)/example.cpp \

BOBJ ?=
OBJ  := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEP  := $(patsubst $(SRC_DIR)/%.cpp,$(DEP_DIR)/%.d,$(SRCS))
BINC ?= ../include
INC  := -I$(INC_DIR) -I$(BINC)
DIRS = $(OBJ_DIR) $(TOBJ_DIR) $(DEP_DIR) $(BIN_DIR)

dirs:
	@$(foreach d, $(DIRS), mkdir -p "$(d)";)

LIBRARIES := criterion
LIBS_ARCHIVES ?=

# -------
# Criterion
# -------
CRITERION_DEPO       = $(LIBS_DIR)/toml98
CRITERION_TARGET_DIR = $(OFFICE_DIR)/toml98
CRITERION_DOWN_DIR   = $(CRITERION_TARGET_DIR)/down
CRITERION_BIN_DIR    = $(CRITERION_TARGET_DIR)/bin
CRITERION_DIRS       = $(CRITERION_OBJ_DIR) $(CRITERION_TOBJ_DIR) \
                       $(CRITERION_DEP_DIR) $(CRITERION_BIN_DIR)
CRITERION_ARCHIVE    = $(CRITERION_BIN_DIR)/lib/libcriterion.so
CRITERION_INC_DIR    = $(CRITERION_BIN_DIR)/include

LIBS_ARCHIVES += $(CRITERION_ARCHIVE)
INC += -isystem $(CRITERION_INC_DIR)

criterion: $(CRITERION_ARCHIVE)
$(CRITERION_ARCHIVE):
	@printf "$(BOLD)Downloading criterion library...$(RESET)\n"
	@mkdir -p "$(CRITERION_DOWN_DIR)" "$(CRITERION_BIN_DIR)"
	@curl -L -o "$(CRITERION_DOWN_DIR)/criterion.tar.xz" \
	  https://github.com/Snaipe/Criterion/releases/download/v2.4.3/criterion-2.4.3-linux-x86_64.tar.xz
	@tar -xf "$(CRITERION_DOWN_DIR)/criterion.tar.xz" \
	  -C "$(CRITERION_BIN_DIR)/" --strip-components=1
	@printf "$(BOLD)Downloaded criterion:$(RESET) $(GREEN)$(CRITERION_ARCHIVE)$(RESET)\n"

compile_commands.json: Makefile $(SRCS)
	@echo "Generating compile_commands.json ..."
	@python3 ../../../scripts/generate_compile_commands.py \
		--output $@ \
		--directory $(CURDIR) \
		--compiler $(CCX) \
		$(foreach f,$(CCX_FLAGS),--cflags=$(f)) \
		$(foreach f,$(CFLAGS),--cppflags=$(f)) \
		--cppflags='-DMON_NET_TESTS' \
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
	@printf "$(BOLD)Compiled$(RESET) $(YELLOW)test$(RESET) $(BLUE)$<$(RESET) -> $(GREEN)$@$(RESET) $(BOLD)$(RED)$(DEP_DIR)/$*.d$(RESET)\n"

$(BIN_DIR)/$(NAME): $(LIBS_ARCHIVES) $(OBJ)
	@mkdir -p "$(@D)"
	@$(CCX) -o "$@" $(LDX_FLAGS) $(LDFLAGS) \
		-Wl,-rpath,'$$ORIGIN/$(CRITERION_BIN_DIR)/lib' \
		$(OBJ) $(LIBS_ARCHIVES)
	@printf "$(BOLD)Linked$(RESET) $(GREEN)$(NAME)$(RESET)$(BOLD)" at "$(RESET)$(GREEN)$@$(RESET)\n"

$(NAME): .linkflag_$(MODE) $(BIN_DIR)/$(NAME)
	@cp $(BIN_DIR)/$(NAME) ./$(NAME)
	@printf " $(BOLD)Copied$(RESET) $(GREEN)$(NAME)$(RESET) $(GREEN)to project root.$(RESET)\n"

format:
	@printf "$(BOLD)Formatting source files...$(RESET)\n"
	@find . -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.tpp' \) -exec clang-format -i {} +; true

clang-tidy:
	clang-tidy $(SRCS) -- $(CCX_FLAGS) $(INC)

clean:
	@$(RM) -rfv "$(OBJ_DIR)" "$(TOBJ_DIR)"

fclean:
	@$(RM) -rf "$(ORIGIN_DIR)"
	@$(RM) -f "./$(NAME)"
	@$(RM) -f .linkflag_*

re: fclean all

.PHONY: all clean fclean re dirs format clang-tidy
-include $(DEP)
