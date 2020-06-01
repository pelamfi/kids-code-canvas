[@react.component]
let make = () => {
  <div className="codeEditorRow">
  <div className="leftCell"/>
  
  <div className="codeEditorCell">
    <textarea className="codeEditor">{React.string("x = t")}</textarea>
  </div>
  </div>;
};