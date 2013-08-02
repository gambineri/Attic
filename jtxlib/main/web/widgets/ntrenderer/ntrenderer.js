var jsNTTable = (document.all ? document.all['NTTID'] : document.getElementById ? document.getElementById('NTTID') : null);
var jsNTRows  = (jsNTTable == null ? null : jsNTTable.tBodies[0].rows);
var jsHLRow   = -1; /* row index of the selected (highlighted) node */
  
function getRowIndex(cell) 
{
  /* Returns the 0-based row index */
  try {return document.all ? (cell.parentElement.sectionRowIndex) : (cell.parentNode.sectionRowIndex);} 
  catch(e) {return null;}
}

function getNodeDepth(node_row)     {return jsNTRows[node_row].cells[0].innerText;}
function getNodeId(node_row)        {return jsNTRows[node_row].cells[1].innerText;}
function getParentNodeId(node_row)  {return jsNTRows[node_row].cells[2].innerText;}
function getECStatus(node_row)      {return jsNTRows[node_row].cells[3].innerText;}
function getVisib(node_row)         {return jsNTRows[node_row].cells[4].innerText;}
function getCLoaded(node_row)       {return jsNTRows[node_row].cells[5].innerText;}
function getECCell(node_row)        {return jsNTRows[node_row].cells[(5 + Number(jsNTRows[node_row].cells[0].innerText))];}
function getLabelCell(node_row)     {return jsNTRows[node_row].cells[(7 + Number(jsNTRows[node_row].cells[0].innerText))];}

function setECStatus(node_row, val) {jsNTRows[node_row].cells[3].innerText = val;}
function setVisib(node_row, val)    {jsNTRows[node_row].cells[4].innerText = val;}
function setCLoaded(node_row, val)  {jsNTRows[node_row].cells[5].innerText = val;}

function _jtxntr_runHiddenPage(srcpage)
{
  var ifr = document.getElementById('hiddenAction');
  if (ifr)
    ifr.parentNode.removeChild(ifr);

  var ifr = document.createElement('<iframe>');
  ifr.id = 'hiddenAction';
  ifr.style.visibility = 'hidden';
  ifr.src = srcpage;
  document.body.appendChild(ifr);
}

function expandGraphics(currow, cell)
{
  if (cell.children('imgExpColl'))
  {
    cell.children('imgExpColl').src = imgExpanded.src;
    jsNTRows[currow].children('tdLabel').style.fontWeight = 'normal';
  }
  if (jsNTRows[currow].children('tdFolderImg').children('imgFolder'))
    jsNTRows[currow].children('tdFolderImg').children('imgFolder').src = imgFolderExpanded.src;
}

function collapseGraphics(currow, cell)
{
  if (cell.children('imgExpColl'))
    cell.children('imgExpColl').src = imgCollapsed.src;
  if (jsNTRows[currow].children('tdFolderImg').children('imgFolder'))
    jsNTRows[currow].children('tdFolderImg').children('imgFolder').src = imgFolderCollapsed.src;
}

function expand_all()
{
  var i = 0;
  while (i < jsNTRows.length)
  {
    if (getECStatus(i) != '' && getCLoaded(i))
    {
      setECStatus(i, '-');
      expandGraphics(i, getECCell(i));
    }
    setVisib(i, '1');
    jsNTRows[i].style.display = 'block';
    i++;
  }
}

function collapse_all()
{
  setECStatus(0, '+');
  setVisib(0, '1');
  collapseGraphics(0, getECCell(0));
  jsNTRows[0].style.display = 'block';

  var i = 1;
  while (i < jsNTRows.length)
  {
    if (getECStatus(i) != '')
    {
      setECStatus(i, '+');
      collapseGraphics(i, getECCell(i));
    }
    setVisib(i, '0');
    jsNTRows[i].style.display = 'none';
    i++;
  }
}

function debug()
{
  var s;
  for(k=0; k < jsNTRows.length; k++)
    s += getVisib(k) + '-';
  alert(s);
}

function nt_expand_collapse(cell)
{
  if (jsNTRows == null)
  {
    alert('Bad NaryTree configuration.');
    return;
  }

  var currow = getRowIndex(cell);

  if (getCLoaded(currow) == '0')
  {
    setCLoaded(currow, '1');
    setECStatus(currow, '-');
    expandGraphics(currow, cell);
    _jtxntr_runHiddenPage(ChildNodesStreamer + '&nodeid=' + getNodeId(currow) + '&noderow=' + currow);
    return;
  }

  if (getECStatus(currow) == '+') //Node must be expanded
  {
    setECStatus(currow, '-');
    expandGraphics(currow, cell);
    expand_node(currow);
  }
  else  //Node must be collapsed
  {
    setECStatus(currow, '+');
    collapseGraphics(currow, cell);
    collapse_node(currow);
  }
}

function expand_node(rowid)
{
  var i = rowid + 1;
  var cur_depth = getNodeDepth(i);

  while (i < jsNTRows.length && getNodeDepth(i) >= cur_depth)
  {
    if (getNodeDepth(i) == cur_depth)
    {
      setVisib(i, '1');
      jsNTRows[i].style.display = 'block';
      if (getECStatus(i) == '-')
        expand_node(i);
    }
    i++;
  }
}

function collapse_node(rowid)
{
  var i = rowid + 1;
  var cur_depth = getNodeDepth(i);

  while (i < jsNTRows.length && getNodeDepth(i) >= cur_depth)
  {
    setVisib(i, '0');
    jsNTRows[i].style.display = 'none';
    i++;
  }
}

function highlightNodeLabel(cell)
{ 
  var nodeLabelPTE = cell.parentTextEdit;

  if (nodeLabelPTE != null) 
  {
    if (jsHLRow >= 0)
      getLabelCell(jsHLRow).style.textDecoration = 'none';
    
    jsHLRow = getRowIndex(cell);
    
    var oTextRange = nodeLabelPTE.createTextRange();
    oTextRange.moveToElementText(cell);
    oTextRange.collapse();
    for (i=0; i < cell.innerText.length; i++)
      oTextRange.expand("character");
    oTextRange.select();
    cell.style.textDecoration = 'underline';
  }
}

