
[Setup]
AppName                 = CPU8051
AppVerName              = CPU8051_1_0_0
DefaultDirName          = {pf}\8051
DisableProgramGroupPage = yes
OutputBaseFilename      = setup_cpu8051_1_0_0

[Dirs]
Name: "{app}\obj"

[Files]
; This inno file
Source: "cpu8051.iss";                                 DestDir: "{app}"

; Linux/cygwin make file, and top level scripts
Source: "makefile";                                    DestDir: "{app}"
Source: "asm8051";                                     DestDir: "{app}"
Source: "convert.sed";                                 DestDir: "{app}"
Source: "runtest";                                     DestDir: "{app}"

; Documentation
Source: "doc\README.pdf";                              DestDir: "{app}\doc"; Flags: isreadme

; Visual studio files for 2010 express
Source: "msvc\8051.sln";                               DestDir: "{app}\msvc"
Source: "msvc\8051\8051.vcxproj";                      DestDir: "{app}\msvc\8051"
Source: "msvc\8051\8051.vcxproj.filters";              DestDir: "{app}\msvc\8051"


; Pre-compiled executable for windows
Source: "msvc\Release\8051.exe";                       DestDir: "{app}"

; Source code
Source: "src\8051.h";                                  DestDir: "{app}\src"
Source: "src\cpu8051.c";                               DestDir: "{app}\src"
Source: "src\cpu8051.h";                               DestDir: "{app}\src"
Source: "src\execute.c";                               DestDir: "{app}\src"
Source: "src\getopt.c";                                DestDir: "{app}\src"
Source: "src\inst.c";                                  DestDir: "{app}\src"
Source: "src\read_ihx.h";                              DestDir: "{app}\src"
Source: "src\read_ihx.c";                              DestDir: "{app}\src"


; Model test source code
Source: "test\acall\acall.asm";                        DestDir: "{app}\test\acall\acall.asm"
Source: "test\add\add.asm";                            DestDir: "{app}\test\add\add.asm"
Source: "test\addc\addc.asm";                          DestDir: "{app}\test\addc\addc.asm"
Source: "test\anl\anl.asm";                            DestDir: "{app}\test\anl\anl.asm"
Source: "test\cjne\cjne.asm";                          DestDir: "{app}\test\cjne\cjne.asm"
Source: "test\clr\clr.asm";                            DestDir: "{app}\test\clr\clr.asm"
Source: "test\cpl\cpl.asm";                            DestDir: "{app}\test\cpl\cpl.asm"
Source: "test\da\da.asm";                              DestDir: "{app}\test\da\da.asm"
Source: "test\dec\dec.asm";                            DestDir: "{app}\test\dec\dec.asm"
Source: "test\div\div.asm";                            DestDir: "{app}\test\div\div.asm"
Source: "test\djnz\djnz.asm";                          DestDir: "{app}\test\djnz\djnz.asm"
Source: "test\inc\inc.asm";                            DestDir: "{app}\test\inc\inc.asm"
Source: "test\jb\jb.asm";                              DestDir: "{app}\test\jb\jb.asm"
Source: "test\jc\jc.asm";                              DestDir: "{app}\test\jc\jc.asm"
Source: "test\jmp\jmp.asm";                            DestDir: "{app}\test\jmp\jmp.asm"
Source: "test\jz\jz.asm";                              DestDir: "{app}\test\jz\jz.asm"
Source: "test\mov\mov.asm";                            DestDir: "{app}\test\mov\mov.asm"
Source: "test\movc\movc.asm";                          DestDir: "{app}\test\movc\movc.asm"
Source: "test\mul\mul.asm";                            DestDir: "{app}\test\mul\mul.asm"
Source: "test\orl\orl.asm";                            DestDir: "{app}\test\orl\orl.asm"
Source: "test\push\push.asm";                          DestDir: "{app}\test\push\push.asm"
Source: "test\reti\reti.asm";                          DestDir: "{app}\test\reti\reti.asm"
Source: "test\rl\rl.asm";                              DestDir: "{app}\test\rl\rl.asm"
Source: "test\setb\setb.asm";                          DestDir: "{app}\test\setb\setb.asm"
Source: "test\subb\subb.asm";                          DestDir: "{app}\test\subb\subb.asm"
Source: "test\xch\xch.asm";                            DestDir: "{app}\test\xch\xch.asm"
Source: "test\xrl\xrl.asm";                            DestDir: "{app}\test\xrl\xrl.asm"


