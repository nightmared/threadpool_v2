project('webserv', 'c', default_options : ['c_std=c11'])
shared = ['threadpool.c', 'webserv.c', 'list.c', 'aho_tree.c', 'http.c']
deps = [dependency('threads')]
main = executable('webserv', 'main.c', shared, dependencies: deps)
client = executable('client', 'client.c', shared, dependencies: deps)
