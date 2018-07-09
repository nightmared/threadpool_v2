project('webserv', 'c', default_options : ['c_std=c11'])
shared = ['threadpool.c']
deps = [dependency('threads')]
main = executable('webserv', 'main.c', shared, dependencies: deps)
client = executable('client', 'client.c', objects: main.extract_objects(shared), dependencies: deps)
