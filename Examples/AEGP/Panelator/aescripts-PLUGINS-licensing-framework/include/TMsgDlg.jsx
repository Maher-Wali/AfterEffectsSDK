{
var dlgResult = {};

function aesll_showDialog(titleText, msgText, btn1Text, btn2Text, btn3Text, showInput, inputText) {
	var dlg = new Window("dialog", titleText, undefined, { resizeable: false, });
	var res = "Group { \
		orientation: 'column', alignment: ['fill', 'fill'], alignChildren: ['left', 'center'], margin: 10, spacing: 5, \
		gpMessageText: Group { \
			orientation: 'row', \
			alignment: ['left', 'fill'], \
			alignChildren: ['left', 'center'] \
			margin: 0, spacing: 5, \
			txt: StaticText { properties: { multiline: true }, text: 'Message', preferredSize: [470, 30] }, \
		}, \
        gpInputText: Group { \
            orientation: 'row', alignment: ['center', 'fill'], alignChildren: ['center', 'center'], margin: 0, spacing: 5, \
            inp: EditText { properties: { multiline: false }, preferredSize: [470, -1] }, \
        }, \
		gpButtons: Group { \
			orientation: 'row', alignment: ['fill', 'fill'], alignChildren: ['fill', 'bottom'], margin: 0, spacing: 5, \
			btn1: Button { text: 'b1', }, \
			btn2: Button { text: 'b2', }, \
			btn3: Button { text: 'b3', properties: { name: 'ok' } }, \
		}, \
        };";
    var panel = dlg.add(res);
    panel.gpButtons.btn1.onClick = function() { dlgResult.returnValue = 1; dlgResult.btnSelected = 1; if (showInput) dlgResult.inputText = panel.gpInputText.inp.text; dlg.close(); };
    panel.gpButtons.btn2.onClick = function() { dlgResult.returnValue = 1; dlgResult.btnSelected = 2; if (showInput) dlgResult.inputText = panel.gpInputText.inp.text; dlg.close(); };
    panel.gpButtons.btn3.onClick = function() { dlgResult.returnValue = 1; dlgResult.btnSelected = 3; if (showInput) dlgResult.inputText = panel.gpInputText.inp.text; dlg.close(); };
   
    if (dlg instanceof Window) {
        dlgResult.returnValue = 0; 
        dlgResult.btnSelected = -1;
        dlgResult.inputText = "";
        dlg.center();
        panel.gpMessageText.txt.text = msgText;
        if (btn1Text === "") panel.gpButtons.remove(panel.gpButtons.btn1); else panel.gpButtons.btn1.text = btn1Text;
        if (btn2Text === "") panel.gpButtons.remove(panel.gpButtons.btn2); else panel.gpButtons.btn2.text = btn2Text;
        if (btn3Text === "") panel.gpButtons.remove(panel.gpButtons.btn3); else {
			panel.gpButtons.btn3.text = btn3Text;
			panel.gpButtons.btn3.name = "ok";
		}
        if (!showInput) panel.gpInputText.remove(panel.gpInputText.inp); else panel.gpInputText.inp.text = inputText;
        dlg.layout.layout(true);
        dlg.show();
    };
};


function runDlg() {
    aesll_showDialog("##DLG_TITLE##", "##DLG_MSG##", "##DLG_BTN1##", "##DLG_BTN2##", "##DLG_BTN3##", "##DLG_SHOWINPUT##"=="1", "##DLG_INPUT##");
    
    return "" + dlgResult.returnValue + "|" + dlgResult.btnSelected + "|" + dlgResult.inputText;
}

runDlg();

}