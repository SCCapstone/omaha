function getInformation(dev, comp)
{
    var request = new XMLHttpRequest();
    var url = "componentData.json";
    
    request.open("GET", url, false);
    request.overrideMimeType("application/json");
    request.send();
    
    var jsonData = JSON.parse(request.responseText);
    writeInformation(jsonData, dev, comp);
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