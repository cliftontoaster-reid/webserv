# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jfranc <jfranc@student.42nice.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/19 11:30:46 by jfranc            #+#    #+#              #
#    Updated: 2026/05/19 11:30:48 by jfranc           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CCX = clang++

CCX_FLAGS ?= -Wall -Wextra -Werror -std=c++98 -pipe -MMD -MP
LDX_FLAGS ?= 

DEBUG_CFLAGS	 = -g3 -O0 -fno-omit-frame-pointer -D_GLIBCXX_DEBUG \
								 -fsanitize=address,undefined -fstack-protector-strong
DEBUG_LDFLAGS	= -fsanitize=address,undefined

RELEASE_CFLAGS = -O3 -DNDEBUG -D_FORTIFY_SOURCE=2 \
								 -fstack-protector-strong -flto -ffunction-sections -fdata-sections
RELEASE_LDFLAGS = -flto -Wl,--gc-sections -Wl,-z,relro -Wl,-z,now

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

# -------
# Project
# -------
NAME = toml98
all: $(NAME).a $(NAME).so

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

LIBRARIES :=
LIBS_ARCHIVES :=

# TODO: tests

# TODO: make a script for that
compile_commands.json: Makefile $(SRC) $(INC)
	@echo "Generating compile_commands.json ..."
	@bear -- $(MAKE) fclean all CC=clang++ VERBOSE=1
	@echo "compile_commands.json generated."

.linkflag_$(MODE):
	@$(RM) -f .linkflag_*
	@touch $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D) $(dir $(DEP_DIR)/$*.d)
	@$(CCX) $(CCX_FLAGS) $(CFLAGS) \
		-MF $(DEP_DIR)/$*.d -c $< -o $@ $(INC)
	@printf "$(BOLD)Compiled$(RESET) $(YELLOW)test$(RESET) $(BLUE)$<$(RESET) -> $(GREEN)$@$(RESET) $(BOLD)$(RED)$(DEP_DIR)/$*.d$(RESET)"

$(BIN_DIR)/$(NAME).a: $(OBJ) $(LIBS_ARCHIVES)
	@ar rcs "$@" $(OBJ) $(LIBS_ARCHIVES)
	@printf "$(BOLD)Linked$(RESET) $(GREEN)$(NAME).a$(RESET)$(BOLD)" at "$(RESET)$(GREEN)$@$(RESET)"

$(BIN_DIR)/$(NAME).so: $(OBJ) $(LIBS_ARCHIVES)
	@$(CCX) -shared $(LDFLAGS) -o "$@" $(OBJ) $(LIBS_ARCHIVES)
	@printf "$(BOLD)Linked$(RESET) $(GREEN)$(NAME).so$(RESET)$(BOLD)" at "$(RESET)$(GREEN)$@$(RESET)"

$(NAME).a: .linkflag_$(MODE) $(BIN_DIR)/$(NAME).a
	@cp $(BIN_DIR)/$(NAME) ./$(NAME).a
	@printf "$(BOLD)Copied$(RESET) $(GREEN)$(NAME)$(RESET) $(GREEN)to project root.$(RESET)"

$(NAME).a: .linkflag_$(MODE) $(BIN_DIR)/$(NAME).so
	@cp $(BIN_DIR)/$(NAME) ./$(NAME).so
	@printf "$(BOLD)Copied$(RESET) $(GREEN)$(NAME)$(RESET) $(GREEN)to project root.$(RESET)"

clean:
	@for lib in toml98 mon-cgi mon-http mon-net mon-router; do \
		$(MAKE) -C $(LIBS_DIR)/$$lib clean \
			TARGET_DIR="$(OFFICE_DIR)/$$lib"; \
	done
	@$(RM) -rfv "$(OBJ_DIR)" "$(TOBJ_DIR)"

fclean:
	@$(RM) -rfv "$(ORIGIN_DIR)"
	@$(RM) -fv "./$(NAME).a" "./$(NAME).so"
	@$(RM) -f .linkflag_*

re: fclean all

.PHONY: all clean fclean re dirs
-include $(DEP)
