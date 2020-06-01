
// State which is evaluated by user provided scriplet and then rendered every frame
[@bs.deriving jsConverter]
type evalState = {
  x: float,
  y: float,
  r: float,
  g: float,
  b: float 
};

let evalScripletPrefix: string = {|
"use strict";
var x = 0.0;
var y = 0.0;
var punainen = 0.9;
var vihrea = 0.7;
var sininen = 0.4;
var lapinakyvyys = 1.0;
|};

let evalScripletSuffix: string = {|
return {x, y, r: punainen, g: vihrea, b: sininen};
|};

type evalFunction = (float) => evalState;

let createEvalFunctionJs: (string) => evalFunction = [%bs.raw
  {|
function createEvalFunction(scriptlet) {
    try {  
      var compiled = new Function("t", scriptlet);
      return (t) => {
          try {  
            var result = compiled(t);
            var asRecord = evalStateFromJs(result);
            return asRecord;
          } catch (exception) {
            console.log(exception);
            return (t) => ({x: t, y: 0, r: 1, g: 0, b: 0});
          }          
      }
    } catch (exception) {
      console.log(exception);
      return (t) => ({x: t, y: 0, r: 1, g: 0, b: 0});
    }
}
|}
];

let createEvalFunction = (scriplet: string): evalFunction => {
  let combinedScript = evalScripletPrefix ++ scriplet ++ evalScripletSuffix;
  createEvalFunctionJs(combinedScript);
};

let initial = createEvalFunction("");

let initialState = createEvalFunction("")(0.0);