This is our addition to the Pintos Operating System.

Group:

Huseyin Sert
Brandon Egonmwan
Jesse Batt
__________________________________________________________________________________________________________________________________

In the examples directory, you will come accross our test programs which we used to test if our system calls worked.

These files are:
-mycreate
-myremove
-myopen

To run these files and test the systems calls you will need to compile them in the examples directory. Once they are
compiled go to userprog/build directory and run the following commands:

pintos-mkdisk filesys.dsk --filesys-size=2 	| Creates a simulated disk named filesys.dsk that contains a 2 MB Pintos
					          file system partition

pintos -f -q				   	| Formats the file system partition by passing -f -q on the kernels command
					     	  line. -f = format, -q = exit when format is done

pintos -p ../../examples/echo -a echo -- -q	| Copies files in to the Pintos system to be able to execute

IMPORTANT: Once you made changes to the test files on examples directory, you should remove the existing ones in Pintos' root
	   directory as running the third command will not overwrite the existing file. You can use the myremove test to remove
	   files. For example: pintos run 'myremove [filename]'

