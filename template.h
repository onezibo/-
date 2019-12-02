

#define DOWNLOAD_HTML_HEAD_TEMPLATE \
  "<!DOCTYPE html>\
<html>\
  <head>\
    <meta charset=\"utf-8\" />\
    <title>Download</title>\
  </head>\
  <body>"

#define UPLOAD_HTML_TEMPLATE \
  "<!DOCTYPE html>\
<html>\
  <head>\
    <meta charset=\"utf-8\"/>\
    <title>Upload</title>\
    <script src=\"https://apps.bdimg.com/libs/jquery/2.1.4/jquery.min.js\">\
    </script>\
  </head>\
  <body>\
    <form> <input type =\"file\" id=\"avatar\" name=\"avatar\">\
    <input type=\"text\" id=\"path\" name=\"path\">\
    <button id=\"btn\" type=\"button\">Upload</button>\
    </form>\
  </body>\
  <script>\
    $('button').click(function(){\
      var files = $('#avatar').prop('files');\
      var path = $('#path').val();\
      var data = new FormData();\
      data.append('avatar', files[0]);\
      data.append('path', path);\
      console.log(path);\
      $.ajax({\
        url : '/upload',\
        type : 'POST',\
        data : data,\
        cache : false,\
        processData : false,\
        contentType : false\
      });\
    });\
  </script>\
</html>"