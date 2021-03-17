$ = jQuery;
$(document).ready(function () {
  // Get Url Paramaters 
  var url = new URL(window.location.href + '?keycode=false');
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
    params.set('keycode', keyPad);
    url.search = params.toString();
    if (keyPad.val() == '3045') {
      window.location.href = url;
    } else {
      keyPad.addClass('error');
      keyPad.val('Incorrect Pin');
    }
  });
});