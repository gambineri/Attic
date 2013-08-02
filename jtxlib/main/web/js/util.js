/***
 * General purpose utilities
 ***/

//------------------------------------------------------------------------------
function callPrint(frameName)
{
	frameName.focus();
	window.print();
}

//------------------------------------------------------------------------------
function submitForm(obj, jsp_page, frame_target)
{
	obj.action = jsp_page;
	obj.target = frame_target;
	obj.submit();
}
	
//------------------------------------------------------------------------------
function submitWithEnter(evt, obj, jsp_page, frame_target)
{
  var keyCode = evt.which ? evt.which : evt.keyCode;
  if (keyCode == 13)
    submitForm(obj, jsp_page, frame_target);
}

//------------------------------------------------------------------------------
function deleteIframe(iframeid) 
{
  var ifr = document.getElementById(iframeid);
  if (ifr)
    ifr.parentNode.removeChild(ifr);
}

//------------------------------------------------------------------------------
function createIframe(iframeid) 
{
  var ifr = document.createElement('<iframe>');
  ifr.id = iframeid;
  document.body.appendChild(ifr);
  return ifr;
}

//------------------------------------------------------------------------------
function runHiddenPage(srcpage)
{
  var ifr = document.getElementById('hiddenAction');
  if (ifr)
    ifr.parentNode.removeChild(ifr);

  ifr = createIframe('hiddenAction');
  ifr.style.visibility = 'hidden';
  ifr.src = srcpage;
}

//------------------------------------------------------------------------------
function checkNum(val)
{
  if (isNaN(val))
  {
    alert('Please enter a valid numeric expression.');
    return false;
  }
  else
    return true;
}

//------------------------------------------------------------------------------
function checkNumBlankInput(input_field)
{
  input_field.value = input_field.value.trim(); 
  if (!checkNum(input_field.value)) 
    input_field.value = '';
}

//------------------------------------------------------------------------------
function mustBeNumeric(control) //to be used "onblur"
{
  if (!checkNum(control.value)) 
  {
    control.value = ''; 
    control.focus();
  }
}

//------------------------------------------------------------------------------
function getTableInDocument(doc, tableid)
{
  return (doc.all ? doc.all[tableid] : 
          doc.getElementById ? doc.getElementById(tableid) : null);
}

//------------------------------------------------------------------------------
function popup(url, width, height)
{
  window.open(url, '_blank', 'toolbar=no, status=no, menubar=no, scrollbars=no, width=' + width + ', height=' + height);
}

//------------------------------------------------------------------------------
function popupModal(url, ref, width, height)
{
  return showModalDialog(url, ref, 'dialogWidth: '+ width + 'px; dialogHeight: '+ height +'px; status:no; scroll: 0;');
}

//------------------------------------------------------------------------------
function checklength(str, max)
{
  if (str.length >= max) 
  {
    str = str.substring(0,max);
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
function warnNoPrivileges()
{
  alert('You do not have enough privileges.');
}

//------------------------------------------------------------------------------
function strLeft(str, n) 
{
  var r = '';
  var cut = n < str.length ? n : str.length;
  for (var i = 0 ; i<cut; i++)
    r += str.charAt(i);
  return r;
}

//------------------------------------------------------------------------------
function setCaretToEnd(control) 
{
  if (control.createTextRange) 
  {
    var range = control.createTextRange();
    range.collapse(false);
    range.select();
  }
  else if (control.setSelectionRange) 
  {
    control.focus();
    var length = control.value.length;
    control.setSelectionRange(length, length);
  }
}

//------------------------------------------------------------------------------
function setCaretToStart(control) 
{
  if (control.createTextRange) 
  {
    var range = control.createTextRange();
    range.collapse(true);
    range.select();
  }
  else if (control.setSelectionRange) 
  {
    control.focus();
    control.setSelectionRange(0, 0);
  }
}

//------------------------------------------------------------------------------
function trimString(str)
{
  str = (this != window? this : str);
  return str.replace(/^\s+/g, '').replace(/\s+$/g, '');
}
String.prototype.trim = trimString;

//------------------------------------------------------------------------------
function toITAdate(yyyymmdd)
{
  return (yyyymmdd == "" ? "" : yyyymmdd.substring(6) + "/" + 
                                yyyymmdd.substring(4, 6) + "/" + 
                                yyyymmdd.substring(0, 4));
}

//------------------------------------------------------------------------------
function openNewWinMax(href)
{
  var w = window.screen.availWidth;
  var h = window.screen.availHeight;
  var win = open(href, '_blank', 'resizable=1,status=1,menubar=1,toolbar=1,location=0,scrollbars=1');
  win.moveTo(0, 0);
  win.resizeTo(window.screen.availWidth, window.screen.availHeight)
}

//------------------------------------------------------------------------------
function getOptionIndexByValue(select_obj, opt_val)
{
  for (i=0; i<select_obj.options.length; i++)
    if (select_obj.options[i].value == opt_val)
      return i;
  
  return -1;
}

//------------------------------------------------------------------------------
function getSelectedOption(select_obj)
{
  return select_obj.options[select_obj.selectedIndex].value;
}

//------------------------------------------------------------------------------
function setSelectedOption(selectobj, optvalue)
{
  // Set to 'selected' the option in the given select
  for (i=0; i < selectobj.length; i++)
    if (selectobj.options[i].value == optvalue)
    {
      selectobj.selectedIndex = i; 
      break;
    }
}

//------------------------------------------------------------------------------
function replaceSelectOptionValue(select_obj, existing_optval, new_val)
{
  select_obj.options[getOptionIndexByValue(select_obj, existing_optval)].value = new_val;
}

//------------------------------------------------------------------------------
function replaceSelectOptionText(select_obj, existing_optval, new_val)
{
  select_obj.options[getOptionIndexByValue(select_obj, existing_optval)].text = new_val;
}

//------------------------------------------------------------------------------
function formatCurrency(num) 
{
  num = num.toString().replace(/\$|\,/g,'');
  if(isNaN(num))
    num = "0";

  sign = (num == (num = Math.abs(num)));
  num = Math.floor(num*100+0.50000000001);
  cents = num%100;
  num = Math.floor(num/100).toString();

  if(cents<10)
    cents = "0" + cents;

  for (var i = 0; i < Math.floor((num.length-(1+i))/3); i++)
    num = num.substring(0,num.length-(4*i+3))+','+ num.substring(num.length-(4*i+3));

  return (((sign)?'':'-') + '$' + num + '.' + cents);
}

