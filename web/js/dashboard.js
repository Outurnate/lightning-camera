/* stormwatch
 * Copyright (C) 2020 Joe Dillon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

setInterval(function()
{
  $("#live").attr("src", "live.jpeg?" + (new Date()).getTime());
}, 1000);

var video = document.getElementById("videoPreview");
var source = document.createElement("source");
video.appendChild(source);
video.type = "video/mp4";

var updateVideos = function()
{
  $("#loader").show();
  $("#videoList").empty();
  $.getJSON("clips", function(data)
  {
    var template = $('#video-template').html();
    $.each(data, function(key, val)
    {
      var title = new Date(Date.parse(val.title));
      $('#videoList').append(template
        .replace("{title}", title)
        .replace("{thumbnail}", val.thumbnail)
        .replace("{url}", val.video)
        .replace("{thumbnail_id}", "thumbnail" + key)
        .replace("{title_id}", "title" + key));
      
      var showHandler = function()
      {
        var video = document.getElementById("videoPreview");
        var source = video.firstChild;

        video.pause();
        source.setAttribute("src", val.video); 
      
        video.load();
        video.play();
      };

      $("#thumbnail" + key).click(showHandler);
      $("#title" + key).click(showHandler);
    });
    $("#loader").hide();
  });
}

setInterval(updateVideos, 60000);
updateVideos();

$.getJSON("settings", function(data)
{
  $("#inputEdgeDetectionSeconds").val(data.EdgeDetectionSeconds);
  $("#inputDebounceSeconds").val(data.DebounceSeconds);
  $("#inputTriggerDelay").val(data.TriggerDelay);
  $("#inputTriggerThreshold").val(data.TriggerThreshold);
});

$("#saveSettings").click(function()
{
  $.post("settings",
    {
      EdgeDetectionSeconds: $("#inputEdgeDetectionSeconds").val(),
      DebounceSeconds: $("#inputDebounceSeconds").val(),
      TriggerDelay: $("#inputTriggerDelay").val(),
      TriggerThreshold: $("#inputTriggerThreshold").val()
    }
  );
});