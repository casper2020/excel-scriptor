
function round(number, precision) {
    var sig=0;
    if(number<0) sig=1;
    number = Math.abs(number);
    var shift = function (number, precision, reverseShift) {
        if (reverseShift) {
            precision = -precision;
        }
        var numArray = ("" + number).split("e");
        return +(numArray[0] + "e" + (numArray[1] ? (+numArray[1] + precision) : precision));
    };
    var final = shift(Math.round(shift(number, precision, false)), precision, true);
    if(sig==0) return final;
    else return -final;
}


function ceil(number, precision) {
    var sig=0;
    if(number<0) sig=1;
    number = Math.abs(number);
    var shift = function (number, precision, reverseShift) {
        if (reverseShift) {
            precision = -precision;
        }
        var numArray = ("" + number).split("e");
        return +(numArray[0] + "e" + (numArray[1] ? (+numArray[1] + precision) : precision));
    };
    var final = shift(Math.ceil(shift(number, precision, false)), precision, true);
    if(sig==0) return final;
    else return -final;
}


function floor(number, precision) {
    var sig=0;
    if(number<0) sig=1;
    number = Math.abs(number);
    var shift = function (number, precision, reverseShift) {
        if (reverseShift) {
            precision = -precision;
        }
        var numArray = ("" + number).split("e");
        return +(numArray[0] + "e" + (numArray[1] ? (+numArray[1] + precision) : precision));
    };
    var final = shift(Math.floor(shift(number, precision, false)), precision, true);
    if(sig==0) return final;
    else return -final;
}

function colName(n) {
    var ordA = 'A'.charCodeAt(0);
    var ordZ = 'Z'.charCodeAt(0);
    var len = ordZ - ordA + 1;

    var s = "";
    while(n >= 0) {
        s = String.fromCharCode(n % len + ordA) + s;
        n = Math.floor(n / len) - 1;
    }
    return s;
}


function lines(a_this_row, a_col){

    var row = a_this_row.match(/\d+/)[0];
    var col = colName(getColumnRef(a_col)-1);

    var final_val = this[col+row]['x'];
    return final_val;
}


function match(table, c1){

    for( var i=0; i<table.length; i++){
        if(table[i].name==c1) return i;
    }

    return "NaN";
}

function vlookup(table, number, c1, index2, range_lu){

    var index1=0;
    var indexf=0;

    index2 = index2-1;

    if(c1=='genvar1'){
        index1 = 0;
    } else {
        for( var i=0; i<table.length; i++){
            if(table[i].name==c1){
                index1 = i;
                break;
            }
        }
    }
    for( var i=0; i<table[index1].data.length; i++){
        if(table[index1].data[i]==number){
            indexf = i;
            return table[index2].data[indexf];
        }
    }
    if(range_lu==1){
        var best = 0;
        var i_best;
        for( var i=0; i<table[index1].data.length; i++){
            if(table[index1].data[i]<number && table[index1].data[i]>best){
                i_best = i;
                best = table[index1].data[i];
            }
        }

        return table[index2].data[i_best];
    }

    return "NaN";
}

function lookup(a_table, number, c1, c2){
    var index1=0;
    var index2=0;
    var indexf=0;
    var table;

    if(a_table != 'INDIRECT') table = this[a_table];
    else{
        table = this[c1.substring(0, c1.indexOf('['))];
        c1 = c1.substring(c1.indexOf('[')+1, c1.indexOf(']'));
        c2 = c2.substring(c2.indexOf('[')+1, c2.indexOf(']'));
    }

    if(c1=='genvar1'){
        index1 = 0;
    } else {
        for( var i=0; i<table.length; i++){
            if(table[i].name==c1){
                index1 = i;
                break;
            }
        }
    }

    for( var i=0; i<table.length; i++){
        if(table[i].name==c2){
            index2 = i;
            break;
        }
    }
    for( var i=0; i<table[index1].data.length; i++){
        if(table[index1].data[i]==number){
            indexf = i;
            return table[index2].data[indexf];
        }
    }

    var best = 0;
    var i_best;
    for( var i=0; i<table[index1].data.length; i++){
        if(table[index1].data[i]<number && table[index1].data[i]>=best){
            i_best = i;
            best = table[index1].data[i];
        }
    }
    return table[index2].data[i_best];
}

function sumifs(a_mode, a_table, a_column, criteria){

    if(a_mode == 'LINES'){

        var table = a_table;

        var sum_col_ = colName(getColumnRef(a_column)-1);
        var sum_col = [];
        var criteria_col = {};
        var sumT = 0;
        var table_eval = [];

        for(var i=0; i<table.length; i++){
            table_eval[i] = this[table[i]];
        }

        for(var i=0; i<table.length; i++){
            var val = table_eval[i].y;
            var val_c = val.replace(/[^a-zA-Z]+/g, '');
            var val_d = parseInt(val.replace(/[^0-9\.]/g, ''), 10);
            if(val_c == sum_col_) sum_col.push(val_d);
        }

        sum_col.sort(function(a, b){return a-b});

        for (var criteria_it in criteria){
            var criteria_col_ = colName(getColumnRef(criteria_it)-1);
            criteria_col[criteria_it] = [];
            for(var i=0; i<table.length; i++){
                var val = table_eval[i].y;
                var val_c = val.replace(/[^a-zA-Z]+/g, '');
                if(val_c == criteria_col_) criteria_col[criteria_it].push(table_eval[i].x);
            }
        }

        var rows = sum_col.length;

        for(var row = 0; row < rows; row++) {
            var match = true;
            for (var criteria_it in criteria){
                try {
                    var a = criteria_col[criteria_it][row];
                } catch(err) { break; }
                var b = criteria[criteria_it];
                if(a != b){
                    match = false;
                    break;
                }
            }
            if ( true == match ) {
                sumT = sumT + this[sum_col_ + sum_col[row]]['x'];
            }
        }

        if(isNaN(sumT)) return 0;
        else return sumT;
    }

    else{

        var table = a_table

        var index_it = NaN;
        var sumT = 0;
        var sum_col_index = 0;
        var criteria_col_index = 0;

        for( var i=0; i<table.length; i++){
            if(table[i].name==a_column){
                sum_col_index = i;
                break;
            }
        }

        var rows = table[sum_col_index].data.length;

        for(var row = 0; row < rows; row++) {
            var match = true;
            for (var criteria_it in criteria){
                for( var i=0; i<table.length; i++){
                    if(table[i].name==criteria_it){
                        index_it = i;
                        break;
                    }
                }
                criteria_col_index = index_it;

                if ( table[criteria_col_index].data[row] != criteria[criteria_it] ) {
                    match = false;
                    break;
                }
            }
            if ( true == match ) {
                sumT = sumT + table[sum_col_index].data[row];
            }
        }

        if(isNaN(sumT)) return 0;
        else return sumT;
    }
}

function sumif(a_mode, a_table, a_column_sum, a_column_crit, criteria){

    if(a_mode == 'LINES'){

        return "NaN";

    }
    else{
        var sumT = 0;
        var sum_col_index = 0;
        var crit_col_index = 0;
        var table = a_table;

        for( var i=0; i<table.length; i++){
            if(table[i].name==a_column_sum){
                sum_col_index = i;
                break;
            }
        }

        for( var i=0; i<table.length; i++){
            if(table[i].name==a_column_crit){
                crit_col_index = i;
                break;
            }
        }

        var rows = table[sum_col_index].data.length;

        for(var row=0; row<rows; row++){
            if(eval(''+table[crit_col_index].data[row] + criteria)){
              sumT = sumT + table[sum_col_index].data[row];
            }
        }

        return sumT;
    }
}

function find(str, arg1, arg2){
    if(isNaN(str)) return "NaN";
    if(str == "" || arg1 == ""){
        return 1;
    }
    else {
        return str.indexOf(arg1, arg2-1)+1;
    }
}

function isError(val){
    if(val == ("NaN" || undefined || null)) return true;
    else return false;
}

function toDate(dateStr) {
    const [day, month, year] = dateStr.split("-");
    return newDate(year, month, day);
}

function newDate(year, month, day){
    var date_i = -2208988800000;

    month = month+'';
    day=day+'';
    if(month.length == 1) month = '0'+month;
    if(day.length == 1) day = '0'+day;
    var fdate = month+'-'+day+'-'+year;

    var date_now = (new Date(fdate)).getTime();

    return Math.round(((date_now - date_i)/ 86400000)+2);
}

function getYear(dateStr){
    var n_date = getJsDateFromExcel(dateStr);

    return n_date.getFullYear();
}

function getMonth(dateStr){
    var n_date = getJsDateFromExcel(dateStr);

    return n_date.getMonth();
}

function getDay(dateStr){
    var n_date = getJsDateFromExcel(dateStr);

    return n_date.getDay();
}

function getJsDateFromExcel(excelDate) {
    return new Date((excelDate - (25567 + 2))*86400*1000);
}

//TODO: TIPO , REFERENCIA , COD
function getResults(){
    var t0 = performance.now();

    var resultJSON = [];
    var condcol = colName(getColumnRef('CONDICAO')-1);

    var lastRow = firstRow+rowCount+1;

    for(var ii=firstRow; ii<lastRow; ii++){
        var cond = this[condcol+ii];
        if(cond != undefined && cond != null){
            if(cond.x != undefined && cond.x != "NaN" && cond.x != null && cond.x != 0){
                var tmp_obj = {};
                var row = parseInt((cond.y).replace(/[^0-9\.]/g, ''), 10);
                for(var i=2; i<sizeOfLines+2; i++){
                    var colname = colName(i-1);
                    var colvar = this[colname+row];
                    if(colvar != undefined && colvar != null){
                        tmp_obj[columnNames[i]] = colvar.x;
                    }
                }
                resultJSON.push(tmp_obj);
            }
        }
    }
    var t1 = performance.now();
    console.log(resultJSON);

    console.log('performance = ' + (t1-t0));
}
