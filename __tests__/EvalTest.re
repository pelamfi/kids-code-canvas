open Jest;
open Eval;

describe("Simple scriplet eval", () => {
  open Expect;

  test("assign a constant to x", () =>
    expect(createEvalFunction("x = 2")(1.0)) |> toEqual({...Eval.initialState, x: 2.0, y: 0.0}))

  test("assign t to y", () =>
    expect(createEvalFunction("y = t")(7.0)) |> toEqual({...Eval.initialState, x: 0.0, y: 7.0}))

  test("assign t derived value to both x and y with line feed", () =>
    expect(createEvalFunction("y = t * 100\nx = t * 2")(0.5)) |> toEqual({...Eval.initialState, x: 1.0, y: 50.0}))

});
