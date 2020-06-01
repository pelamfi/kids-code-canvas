[@react.component]
let make = () => {

  //  let onChange: (x) = x => CodeCanvasState.dispatch(CodeCanvasState.event.ChangeScriptlet(x.target.value))
  let onChange = (e: ReactEvent.Form.t): unit => {
    CodeCanvasState.dispatch(CodeCanvasState.ChangeScriptlet(e->ReactEvent.Form.target##value));
  };

  <div className="codeEditorRow">
  <div className="leftCell"/>
  
  <div className="codeEditorCell">
    <textarea className="codeEditor" onChange={onChange}>{React.string("x = t")}</textarea>
  </div>
  </div>;
};