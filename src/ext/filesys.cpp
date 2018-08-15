#include "filesys.hpp"

namespace ext {
	
	/// Set working dir to 'executable_path + 'rel
	void set_relative_working_dir(char const* rel) 
	{
		// PATH_MAX ~~ 4096
		char buf[PATH_MAX];
		char nwd[PATH_MAX];
		
		auto ret = readlink("/proc/self/exe", buf, PATH_MAX); 
		if (ret == -1) {
			ext::fail("ERROR: readlink error\n");
		}
		
		// remove filename
		char * p = buf + ret;
		while (buf <= p and *p != '/') {
			--p;
		}
		
		auto r = Rng<char>(p, buf + PATH_MAX);
		
		print(r, "/");
		print(r, rel);
		
		//print("DEBUG: self exe is: {}\n", buf);
		
		if (realpath(buf, nwd) == nullptr) {
			ext::fail("ERROR: realpath error: {}\n", strerror(errno));
		}
		
		chdir(nwd);
		
		print("INFO: working dir is now: {}\n", nwd);		
	}

}
