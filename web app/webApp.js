function doGet(e) {
    Logger.log(JSON.stringify(e));
    var result = 'Ok';
    if (e.parameter == 'undefined') {
      result = 'No Parameters';
    }
    else {
      var sheet_id = ''; // Spreadsheet ID
      var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();
      var newRow = sheet.getLastRow() + 1;
      var rowData = [];
  
      for (var param in e.parameter) {
        var value = stripQuotes(e.parameter[param]);
        switch (param) {
          case 'date':
            rowData[0] = value; // Date in column A
            result = 'Date Written on column A';
            break;
          case 'time':
            rowData[1] = value; // Time in column B
            result += ', Time Written on column B';
            break;
          case 'roomState':
            rowData[2] = value; // Random value 1 in column C
            result = ', Room state Written on column C.';
            break;
          default:
            result = "unsupported parameter";
        }
      }
      // Logger.log(JSON.stringify(rowData));
      var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
      newRange.setValues([rowData]);
    }
    return ContentService.createTextOutput(result);
}
  function stripQuotes(value) {
    return value.replace(/^["']|['"]$/g, "");
}