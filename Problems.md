# Problem list
1. Return IR is getting created in a not good way, for example it happens to that we generate:
```
entry:
  %calltmp = call i32 @test_extern_print.1(i32 100)
  ret i32 %calltmp
  ret void
}
```
on a logical basis this isn't a problem, still this is unnecessairy overhead. Code is in FunctionExpressionAST.cpp.

2. Structs, no further comments
3. ScopeManager and scoping in general, needs a HUGE refactor, rethinking and everything. Normally scoping isn't hard but I think I was a little drunk doing the ScopeManager at first. This stage of dev for scoping is kind of pathetic and desperatly needs changes.
4. 'Syntactical Sugar' -> more freedom with syntax (allow loosely typed dt's etc.)
5. Higher salary, lol.