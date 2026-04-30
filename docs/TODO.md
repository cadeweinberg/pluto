# TODO: Pluto

- [] each subcomponent of the compiler needs it's own library.
     This should ease maintinence of the project as a whole,
     as it will make visible the API surface of each component.
     And properly managing the API surface will be what increases
     the modularity of the system, reduce maintinence headaches
     and allow new components to be developed and plugged in to the
     existing system.
  - [] support: holds basic program utilities and platform specific
       abstractions. This is the library which is used to abstract
       the implementation of the compiler away from the differences
       between C on Linux, Windows, macOS, BSD, etc...
  - [] context: holds the context specific information to the current
       run of the compiler. This is where the incoming arguments to
       the program are parsed, where the options controlling the
       compilation process are decided, and where the process of compilation
       as a whole is orchestrated. It defines Context, ProgramOptions,
       StringInterner, TypeInterner.
  - [] frontend: holds the frontend tokenization and parsing logic
       which is used to construct the TIIR from input source code.
       It defines, Token, Lexer, Parser, etc.
  - [] TIIR: holds the target independent intermediate representation.
       This is the library which defines the API surface of the
       representation, the different passes of the compiler will
       interact via this API, instead of knowing the details of the
       TIIR directly. if the API is well-designed, this is possible.
       Theoretically we should be able to swap out the underlying
       memory representation of the TIIR and have the rest of the
       compiler still work.
       I am conceptualizing it as defining Instruction, Operand, Function,
       Module,
  - [] passes: holds the API of the various passes over the TIIR, which
       either verify, mutate, or lower the TIIR to the TSIR. It is the
       boundary between the the TIIR and the TSIR, as well as the
       implementation of the Code Generator.
       I am conceptualizing it as defining objects such as PassManager,
       VerifyModulePass, DominatorPass, RegisterAllocationPass,
       StrengthReductionPass, FunctionInliningPass, LoopUnrollingPass,
       SerializePass, etc.
  - [] TSIR: holds the target specific intermediate representation.
       This is the library which defined the API surface of the
       TSIR, and abstracts the different passes of the compiler away
       from the details of which particular target architecture is
       being lowered to. It is simultaneously the API boundary of the
       Target Architecture libraries. Such that a new target architecture
       may be supported by providing the implementation details of the
       API, and in this way new targets may be added to the compiler.
       and these new targets can reuse the same optimizations available.
       I am conceptualizing it as defining objects such as:
       TargetInstruction, TargetFunction, TargetObjectFile, TargetDebugInformation,
       etc,

- [] We don't provide a runtime library ourselves. That is the job of the language
     implemented on top of exp. This program is more akin to an assembler that just
     creates the object files to then subsequently be linked together.
