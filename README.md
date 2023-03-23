# Overview
* This PinTool allows users to add markers in the code.
* As an example use case, the code also selectively instruments the branch instructions based on whether they lie in the region of interest or not.
* The region of interest is marked by two special marker values: START and END which are internally mapped to -1 and -2 in the markers.h file.
* Any user can include the markers.h file in their code and use the add\_marker API. This header defines a macro to add a specific sequence of
  instructions in the binary which is the recognized by Pin. The sequence of instructions is:
  ```
  mov    $0x1e6c,%rcx
  mov    $0xffffffffffffffff,%rdx
  xchg   %bx,%bx
  ```
  The first line stores a magic value to the RCX register. This magic value is defined in the `marker.h` file. You can change this magic value if
  you find that Pin is detecting markers which were not specified by you by simply editing the `marker.h` file. If you do this, make sure to pass the
  new magic value as an argument to the MarkerTracer pin tool. The second line stores the user specified macro in the RDX register. Finally, the last line
  performs an xchng operation which is then detected by Pin.

# How to compile the MarkerTracer tool:
1. Install and setup Pin
2. Copy the `MarkerTracer` directory to `source/tools` within the Pin installation directory.
3. Go to the `source/tools/MarkerTracer` directory in the Pin installation.
4. Build the tool using the commands specified in the `README` file in the `MarkerTracer` directory.

# How to add markers to your code:
1. Include the `marker.h` file in your code.
2. Modify the build system to ensure that your compiler can find this file while compiling your code.
3. Build your code.

Note: that the `marker.h` file provides a macro named `add_marker` which allows you to add macros to your code. It also lets you add an argument
to this macro which can then be read by Pin. This macro gets stored in the RDX register which can then be read at runtime by Pin. There are two
special arguments which are also defined called START and END which map to -1 and -2 repectively. You can use these macros to mark the region of
interest. In the provided example, the region of interest is the `main` function and within that region of interest the PinTool selectively
processes the branch instructions. Please refer to the attached example in the `pin_marker_test` dirctory to get an idea of how to use this API
and how to configure the build system.

# How to run the test program with Pin:
1. Compile the MarkerTracer PinTool
2. Compile the test program present in `pin_marker_test` by running `make`
3. Go to the Pin installation directory and run pin with appropriate commands
```
./pin -t source/tools/MarkerTracer/obj-intel64/markertrace.so -- path/to/the/compiled/executable/in/pin_marker_test
```
