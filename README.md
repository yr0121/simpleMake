Makeis a useful utility which builds executable programs or libraries from source files based on an inputmakefilewhich includes information on how to build targets (e.g.  executable programs or libraries).  The makefile specifies adependency graph that governs how targets should be built. 
This is a simple version of Make(make4061) which 
1) reads the makefile, 
2) follows the specification in the file to construct the dependence graph and
3) builds the targets usingfork, execandwaitin a controlled fashion just like standard Make in Linux usingthe graph.  
This make4061 will use the dependence graph to build any targets (e.g.  compile files) in the proper order.  The command to finally build the target can beexecuted once all of its dependencies have been resolved (i.e. those targets have been built and so on).

How to run:
Compile the code with the help of make utility of UNIX. 
run:./make4061 [options]
This make4061 will support the following options:
1.  -f filename: filename will be the name of the makefile, otherwise the default name “Makefile” is assumed.
2.  specificTarget: specificTarget will be the name of any single target in the Makefile.
3.  -h: print usage info