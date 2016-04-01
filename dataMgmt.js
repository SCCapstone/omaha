function getInformation(dev, comp)
{
    var request = new XMLHttpRequest();
    var url = "componentData.json";
    
    request.onreadystatechange = function()
    {
        if (request.readyState == 4 && request.status == 200)
        {
            var json = JSON.parse(request.responseText);
            writeInformation(json, dev, comp);
        }
    };
    
    request.open("GET", url, true);
    request.overrideMimeType("application/json");
    request.send();
}

function writeInformation(jsonData, device, component)
{
    console.log("writeInformation('"+device+"', '"+component+"')");
    
    if (jsonData === null)
    {
        console.log("Received null data.");
        return;
    }
    
    var componentData = jsonData[device][component];
    var titleBar = document.getElementById("title");
    var manf = document.getElementById("manu");
    var serial = document.getElementById("serial");
    var dsLink = document.getElementById("datasheet");
    var flavor = document.getElementById("flavor");
    
    if (!componentData)
    {
        titleBar.innerHTML = "No data found for device:" + device + " component:" + component;
        flavor.innerHTML = titleBar.innerHTML;
    }
    else
    {
        titleBar.innerHTML = componentData.title;
        manf.innerHTML = componentData.manf;
        serial.innerHTML = componentData.serial;
        dsLink.href = componentData.datasheet[0];
        dsLink.innerHTML = componentData.datasheet[1];
        flavor.innerHTML = componentData.detail;
    }
}