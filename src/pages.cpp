#include "pages.h"

namespace pages {
    const char* getIndexPage() {
        // Placeholder implementation
return R"---(
<html>
<head>
    <title>KduPro</title>
    <script type="text/javascript">
        function updateValues() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/state", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    console.log("Response: " + xhr.responseText);
                    var response = JSON.parse(xhr.responseText);
                    document.getElementById("light").innerHTML = (response.light == "true") ? "Yes" : "No";
                    document.getElementById("batt").innerHTML = response.batt;
                }
            };
            xhr.send();
        }

        document.addEventListener("DOMContentLoaded", function() {
            updateValues();
        });
    </script>
</head>
<body>
<h1>KduPro</h1>

<div>
    <h2>Values</h2>
    <p>Is light on: <span id="light">loading...</span></p>

    <p>Battery: <span id="batt">loading...</span></p>
</div>

<div>
    <h2>Actions</h2>
    <button onclick="fetch('/lighton', {method: 'GET'})">Turn light on</button>
    <button onclick="fetch('/lightoff', {method: 'GET'})">Turn light off</button>
</div>


</body>
</html>
)---";
    }
}