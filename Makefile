# If you're interested, this Makefile exploits a few tricks

# There is nothing special about the 'all' target you see in many makefiles.
# Rather, make will simply use the first target it finds in the file by deafult
# (in this case 'check').
#
# Normally, make will echo the line it's going to execute. This is usually nice
# so you can see what's going on, but a little silly when the command to execute
# is 'echo'. The @ character stops make from echoing the command it will run.
#
# The diff command returns success if the files are the same, or error if they
# are different. The -y flag gives side-by-side output, which is convenient,
# but prints unconditionally (even if the output is the same). So we first
# check using 'quiet' diff (don't print anything), and if the command fails
# (the ||) we then run diff -y to print useful output. Make will quit the first
# time a command has an error, which is why it won't print "Output matches" if
# diff indicates there is a difference.
check:	run golden.out
	@echo -e "\nCHECKING OUTPUT"
	@diff -q golden.out list.out || diff -y golden.out list.out
	@echo "Output matches. Task complete."

run:	list
	@echo -e "\nRUNNING PROGRAM"
	@./list | tee list.out


# The $^ expands to all of the dependencies
# The $@ expands to the target
# Make was based on perl, can you tell?
#
# Make has a lot of built-in "implicit rules". For example, it knows how to make
# list.o from list.c, even though we never specified that anywhere. Implicit
# rules will respect things like CFLAGS. For really basic tests, you often don't
# even need a makefile!
list:	main.o list.o print_list.o
	$(CC) $^ -o $@

clean:
	rm -f list.o list list.out

# The PHONY directive tells make that it's not actually creating any files named
# check, run, or clean, rather these are command that should run every time.
.PHONY:	check run clean

