NAME			= 	webserv

CXX				= c++
RM				= rm -rf
CXXFLAGS		= -MD -std=c++98 -I. -g

BLUE 	= \033[1;34m
GRN		= \033[1;32m
RED		= \033[0;31m
VIOLET	= \033[0;35m
YELLOW	= \033[1;33m
TICK	= \xE2\x9C\x94
END		= \033[0m

MAIN		=	main Server ListenSocket ClientSocket LocationInfo ConfigurationSingleton \
				Request Response

HEAD		= $(addprefix -I $(HEAD_DIR)/, $(sort $(dir $(SRCS))))
HEAD_DIR	= $(SRC_DIR)
HEADS 		= $(HEAD_DIR)/$(sort $(dir $(SRCS)))/*.hpp

SRC_DIR		= src
OBJ_DIR		= obj
OBJ_SUBDIR =  $(addprefix $(OBJ_DIR)/,$(sort $(dir $(SRCS))))

SRCS		= $(addsuffix .cpp, $(MAIN))
OBJS		= $(addprefix $(OBJ_DIR)/, $(sort $(dir $(SRCS))))
OBJS_BUILD	= $(addprefix $(OBJ_DIR)/, $(SRCS:cpp=o))

all:		$(NAME)

$(NAME):	$(OBJ_DIR) $(OBJS_BUILD) Makefile $(HEADS)
			@$(CXX) $(CXXFLAGS) $(HEAD) -o $(NAME) $(OBJS_BUILD)
			@printf "%-50.40s\r\n"
			@printf "%-38.38s\r\t$(YELLOW)$(TICK)$(GRN) $(NAME) was compiled!$(END)\n\n"
			@echo "\t$(BLUE)Usage: ./$(NAME) [config_file_name] $(END)\n"
			@./$(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp  $(HEADS)  Makefile
			@printf "$(VIOLET)Generating $(NAME)'s objects... %-38.38s\r" $@
			@$(CXX) $(CXXFLAGS) $(HEAD) -o $@ -c $<

$(OBJ_DIR):
			@mkdir -p $(OBJ_SUBDIR)

test:		all
			@./$(NAME)

clean:
			@$(RM) $(OBJ_DIR)
			@echo "\t$(GRN)$(TICK) $(NAME) was cleaned!$(END)"

fclean: 	clean
			@$(RM) $(NAME)
			@echo "\t$(GRN)$(TICK) $(NAME) was fcleaned!$(END)"

re: 		fclean all

.PHONY:		all clean fclean re