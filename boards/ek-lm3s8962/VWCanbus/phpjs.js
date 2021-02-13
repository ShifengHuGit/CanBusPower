sha256(item.blob).then(function (hash) {
    var postData = {
        'content_type': 'application/x-www-form-urlencoded',
        'content_length': item.blob.size,
        'content_sha256': hash
    };
​
    $.ajax({
        url: '/masterfile/getOciSignature',
        type: "POST",
        dataType: 'json',
        data: postData,
        success: function (response_sign) {
            console.log(response_sign);
​
            var path = '/n/' + encodeURIComponent('nrosgec70xcm') +
                '/b/' + encodeURIComponent('sctbucketobject04') +
                '/u/' + item.registerPath +
                "?uploadid=" + item.uploadId + "&uploadPartNum=" + item.uploadPartNum;
            var region = 'ap-tokyo-1';
​
            var uploadInfoUrl = "https://objectstorage." + region + ".oraclecloud.com" + path;
            var xhr = new XMLHttpRequest();
            xhr.open('PUT', uploadInfoUrl, true);
            xhr.setRequestHeader('content-type', 'application/x-www-form-urlencoded');
​
​
            xhr.setRequestHeader('Authorization', response_sign.signature);
​
            xhr.onreadystatechange = function (aEvt) {
                if (xhr.readyState == 4) {
                    if (xhr.status == 200) {
                        console.log(xhr.responseText);
                    }
                }
            }
            xhr.send(file);
        }
    });
​
});

