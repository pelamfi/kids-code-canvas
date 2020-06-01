
// State which is evaluated by user provided scriplet and then rendered every frame
[@bs.deriving jsConverter]
type scriptletState = {
  x: float,
  y: float,
  r: float,
  g: float,
  b: float 
};

let scripletCompilePrefix: string = {|
"use strict";
var x = 0.0;
var y = 0.0;
var punainen = 0.9;
var vihrea = 0.7;
var sininen = 0.4;
var lapinakyvyys = 1.0;
|};

let dcripletCompileSuffix: string = {|
return {x, y, r: punainen, g: vihrea, b: sininen};
|};

type scriptletFunction = (float) => scriptletState;

let compileScriptletJs: (string) => scriptletFunction = [%bs.raw
  {|
function compileScriptletJs(scriptlet) {
    try {  
      var compiled = new Function("t", scriptlet);
      return (t) => {
          try {  
            var result = compiled(t);
            var asRecord = scriptletStateFromJs(result);
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

let compileScriptlet = (scriplet: string): scriptletFunction => {
  let combinedScript = scripletCompilePrefix ++ scriplet ++ dcripletCompileSuffix;
  compileScriptletJs(combinedScript);
};

let initial = compileScriptlet("x = t");

let initialState = initial(0.0);