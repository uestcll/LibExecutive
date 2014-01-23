If you want to use libexecutive.a, you should use "-lpthread -luuid". For example, type the following command:
#g++ test.cpp libexecutive.a -lpthread -luuid