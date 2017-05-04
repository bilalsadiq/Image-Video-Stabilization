$(document).ready(function(){
    var t = setTimeout(requestDiagnostics, 50);
});

function requestDiagnostics(){
    $.getJSON( '/api/diagnostics', function(data){
    })
    .done(function(data){
        if (data.ack == 'ok'){
            $('#diagnostics').html('')
            $('#diagnostics').append('<p>CPU Usage: ' + data.pi.cpu_percent + '</p>')
            $('#diagnostics').append('<p>RAM Consumption: ' + data.pi.used_memory + ' / ' + data.pi.total_memory + ' MB</p>')
        }
    })
    .always(function(){
        var t = setTimeout(requestDiagnostics, 500);
    });
}
