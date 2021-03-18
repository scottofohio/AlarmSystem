$ = jQuery;
$(document).ready(function () {
  // Get Url Paramaters 
  var url = new URL(window.location.origin + '?keycode=false');
  var params = new URLSearchParams(url.search);
  var keyPad = $('#key');
  var inputKeys = [];
  $('.ent').on('click', function () {
    var number = $(this).attr('id');
    if (inputKeys.length <= 3) {
      inputKeys.push(number);
      $("#key").val(function () {
        return this.value + number;
      });
    }
  });
  $('#snd').on('click', function () {
    params.set('keycode', keyPad.val());
    url.search = params.toString();
    if (keyPad.val() == '3045') {
      window.location.href = url;
      console.log(url);
    } else {
      keyPad.addClass('error');
      keyPad.val('Incorrect Pin');
      inputKeys = [];
      x = 1;
      setInterval(function () {
         if (x == 4) { 
             window.location.href = window.location.origin + '?keycode=wrong-pin';
         } else {
            keyPad.val(x);
             x++;
         }
     }, 1000);      
    }
  });
  $('#clr').on('click',function(){
    keyPad.val('');
    inputKeys = [];
    window.location.href = window.location.origin;
  });
});