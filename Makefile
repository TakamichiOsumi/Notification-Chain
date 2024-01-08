CC	= gcc
CFLAGS	= -O0 -Wall
SUBDIRS = Glued-Doubly-Linked-List
MAKE	= make
CLEANUP	= make clean
LIBS	= -L $(CURDIR)/$(SUBDIRS)
MYLIBS	= -lgldll -lpthread
PROGRAM1	= run_nfc_component_test
PROGRAM2	= run_nfc_threads
ALLPROGRAMS	= $(PROGRAM1) $(PROGRAM2)
all: $(SUBDIRS) $(ALLPROGRAMS)

# Prerequisite git submodule for notification chain build
$(SUBDIRS):
	ifneq ($(shell ls | grep $(SUBDIRS)), $(SUBDIRS)) \
	    "$(error $(SUBDIRS) not found.)" \
	endif \

libgldll.a:
	@cd $(SUBDIRS); $(MAKE); cd ../

$(PROGRAM1): notification_chain.o notification_chain_util.o routing_table.o
	$(CC) $(CFLAGS) $(LIBS) $(MYLIBS) nfc_threads_test.c $^ -o $@

$(PROGRAM2): notification_chain.o notification_chain_util.o routing_table.o thread_subscriber.o
	$(CC) $(CFLAGS) $(LIBS) $(MYLIBS) thread_publisher.c $^ -o $@

thread_subscriber.o: thread_subscriber.c
	$(CC) $(CFLAGS) -c $< -o $@

routing_table.o: routing_table.c
	$(CC) $(CFLAGS) -c $< -o $@

notification_chain.o: notification_chain.c notification_chain_util.o libgldll.a
	$(CC) $(CFLAGS) -c $< -o $@

notification_chain_util.o: notification_chain_util.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:test clean

test: $(PROGRAM1)
	@echo "Testing nfc components is successful when it returns 0."
	@./$(PROGRAM1) &> /dev/null && echo ">>> $$?"

clean:
	@cd $(SUBDIRS); $(CLEANUP); cd ../
	@rm -f *.o $(ALLPROGRAMS)
