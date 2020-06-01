open Jest;
open Scriptlet;

describe("Simple scriplet eval", () => {
  open Expect;

  test("assign a constant to x", () =>
    expect(compileScriptlet("x = 2")(1.0)) |> toEqual({...Scriptlet.initialState, x: 2.0, y: 0.0}))

  test("assign t to y", () =>
    expect(compileScriptlet("y = t")(7.0)) |> toEqual({...Scriptlet.initialState, x: 0.0, y: 7.0}))

  test("assign t derived value to both x and y with line feed", () =>
    expect(compileScriptlet("y = t * 100\nx = t * 2")(0.5)) |> toEqual({...Scriptlet.initialState, x: 1.0, y: 50.0}))

});
