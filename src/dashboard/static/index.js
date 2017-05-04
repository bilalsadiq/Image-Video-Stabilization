$(document).ready(function(){
    var t = setTimeout(refreshImage, 33);
    

});
$(document).ajaxComplete(function(){
    var t = setTimeout(refreshImage, 33);
});
function refreshImage(){
    $.getJSON('/api/image', function(data){
        $('#canvas').html('<img src="data:image/jpeg;base64,' + data.img + '" />');
    });
};
