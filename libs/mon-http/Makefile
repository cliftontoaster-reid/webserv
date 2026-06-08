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

CCX_FLAGS ?= -Wall -Wextra -Werror -std=c++98 -pipe -fPIC -MMD -MP
LDX_FLAGS ?= 

DEBUG_CFLAGS	 = -g3 -O0 -fno-omit-frame-pointer -D_GLIBCXX_DEBUG \
								 -fsanitize=address,undefined -fstack-protector-strong
DEBUG_LDFLAGS	= -fsanitize=address,undefined

COVERAGE_CFLAGS  = -O0 -g --coverage
COVERAGE_LDFLAGS = --coverage

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
NAME = mon-http
all: lib$(NAME).a lib$(NAME).so

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
	$(SRC_DIR)/AHttpRequest.cpp \
	$(SRC_DIR)/Http10Request.cpp \
	$(SRC_DIR)/Http10Response.cpp \
	$(SRC_DIR)/HttpMethod.cpp \
	$(SRC_DIR)/HttpVersion.cpp \

OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEP := $(patsubst $(SRC_DIR)/%.cpp,$(DEP_DIR)/%.d,$(SRCS))
INC := -I$(INC_DIR)
DIRS = $(OBJ_DIR) $(TOBJ_DIR) $(DEP_DIR) $(BIN_DIR)

dirs:
	@$(foreach d, $(DIRS), mkdir -p "$(d)";)

LIBRARIES :=
LIBS_ARCHIVES :=

# TODO: tests

compile_commands.json: Makefile $(SRCS)
	@echo "Generating compile_commands.json ..."
	@python3 ../../scripts/generate_compile_commands.py \
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

$(BIN_DIR)/$(NAME).a: $(OBJ) $(LIBS_ARCHIVES)
	@mkdir -p $(BIN_DIR)
	@ar rcs "$@" $(OBJ) $(LIBS_ARCHIVES)
	@printf "$(BOLD)Linked$(RESET) $(GREEN)$(NAME).a$(RESET)$(BOLD)" at "$(RESET)$(GREEN)$@$(RESET)\n"

$(BIN_DIR)/$(NAME).so: $(OBJ) $(LIBS_ARCHIVES)
	@mkdir -p $(BIN_DIR)
	@$(CCX) -shared $(LDFLAGS) -o "$@" $(OBJ) $(LIBS_ARCHIVES)
	@printf "$(BOLD)Linked$(RESET) $(GREEN)$(NAME).so$(RESET)$(BOLD)" at "$(RESET)$(GREEN)$@$(RESET)\n"

lib$(NAME).a: .linkflag_$(MODE) $(BIN_DIR)/$(NAME).a
	@cp $(BIN_DIR)/$(NAME).a ./lib$(NAME).a
	@printf " $(BOLD)Copied$(RESET) $(GREEN)lib$(NAME).a$(RESET) $(BOLD)to project root.$(RESET)\n"

lib$(NAME).so: .linkflag_$(MODE) $(BIN_DIR)/$(NAME).so
	@cp $(BIN_DIR)/$(NAME).so ./lib$(NAME).so
	@printf " $(BOLD)Copied$(RESET) $(GREEN)lib$(NAME).so$(RESET) $(BOLD)to project root.$(RESET)\n"

# -----
# Tests
# -----
test: lib$(NAME).a
	@printf "$(BOLD)Building tests...$(RESET)\n"
	@CCX="$(CCX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LDX_FLAGS="$(LDX_FLAGS)" \
	TARGET_DIR="$(abspath $(TARGET_DIR))" MODE="$(MODE)" \
	BOBJ="$(OBJ)" LIBS_ARCHIVES="../lib$(NAME).a $(LIBS_ARCHIVES)" \
	$(MAKE) -C $(TEST_DIR) all --silent
	@printf "$(BOLD)Built tests$(RESET)\n"

format:
	@printf "$(BOLD)Formatting source files...$(RESET)\n"
	@find . -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.tpp' \) -exec clang-format -i {} +; true

clang-tidy:
	clang-tidy $(SRCS) -- $(CCX_FLAGS) $(INC)

clean:
	@$(RM) -rf "$(OBJ_DIR)" "$(TOBJ_DIR)"

fclean:
	@$(RM) -rf "$(ORIGIN_DIR)"
	@$(RM) -f "./lib$(NAME).a" "./lib$(NAME).so"
	@$(RM) -f .linkflag_*

re: fclean all

.PHONY: all clean fclean re dirs compile_commands.json test format clang-tidy
-include $(DEP)
