CONFIG_FILE = ../../config
include $(CONFIG_FILE)

OBJS   = json_reader.o json_value.o json_writer.o

TEST   = test_lib_json
TEST_OBJS = main.o jsontest.o

all: $(OBJS) $(TEST)

$(OBJS):
	$(CXX) -I../../include $(CFLAGS) -c json_reader.cc
	$(CXX) -I../../include $(CFLAGS) -c json_value.cc
	$(CXX) -I../../include $(CFLAGS) -c json_writer.cc

$(TEST):
	$(CXX) -I../../include $(CFLAGS) -w -c main.cc
	$(CXX) -I../../include $(CFLAGS) -w -c jsontest.cc

test:
	$(CXX) -o $(TEST) $(TEST_OBJS) ../../lib/libapolloron.a $(LDFLAGS)
	@./test_lib_json

json_reader.o: json_reader.cc
json_value.o: json_value.cc
json_writer.o: json_writer.cc

main.o: main.cc
jsontest.o: jsontest.cc

clean:
	$(RM) $(OBJS) $(TEST) $(TEST_OBJS)
