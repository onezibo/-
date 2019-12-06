

#define DOWNLOAD_HTML_HEAD_TEMPLATE \
  "<!DOCTYPE html>\
<html>\
  <head>\
    <meta charset=\"utf-8\" />\
    <script src=\"https://use.fontawesome.com/80a7387260.js\"></script>\
    <title>Download</title>\
  </head>\
  <body>"

#define DOWNLOAD_HTML_TAIL_TEMPLATE "</body>\n</html>"
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
    <input type=\"text\" id=\"path\" name=\"path\" placeholder=\"上传路径，默认为/\">\
    <button id=\"btn\" type=\"button\">Upload</button>\
    </form>\
  </body>\
  <script>\
    $('button').click(function(){\
      var files = $('#avatar').prop('files');\
      var path = $('#path').val();\
      if(path === '') {\
        path = '/';\
      }\
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