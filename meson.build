project('webserv', 'c', default_options : ['c_std=c11'])
shared = ['threadpool.c', 'webserv.c', 'list.c']
deps = [dependency('threads')]
main = executable('webserv', 'main.c', shared, dependencies: deps)
client = executable('client', 'client.c', dependencies: deps)
