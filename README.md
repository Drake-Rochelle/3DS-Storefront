# 3DS Storefront V0.1
3DS Storefront is a homebrew app that allows you to host stores and download files from them straight onto your 3DS!

### HOW TO MAKE CUSTOM STOREFRONT
To make a custom storefront, you don't need to host any servers or anything. All you need is a google drive account. To start, go to your desired google drive and create a new folder. You can name it whatever you want.

<img width="715" height="347" alt="image" src="https://github.com/user-attachments/assets/9c1a6784-2024-44e6-b8d5-18958d0ca924" />

Then, right klick on that folder and click "Share>Share".

<img width="1099" height="568" alt="image" src="https://github.com/user-attachments/assets/2c78bbba-dd0c-4f38-a741-be935f3695e0" />

Set the visibility to "Anyone with link" and click done.

Then, double click this folder to open it. Now, we'll click "New" and then "File Upload". Now, we'll upload all the files we want for our storefront. 

Next, press win+d to go to your desktop (or you can open file explorerer and go to any directory you can easily access). Then, right click and select "New>Text Document". We'll name this "storefront.json". If you can't change the extention, then "storefront.json.txt" is fine. Now, open this file in a text editor or IDE of your choice. In this file, we'll write this:

```
{
"name":["link","file name","/directory"],
"name":["link","file name","/directory"],
"name":["link","file name","/directory"],
...
}
```

Each line coresponds to one of the files you uploaded to google drive. Each "link" should be replaced with a link from the instructions below. Each "file name" should be replaced with the name the file should be downloaded as including the extention (e.g. "game.cia"). Each "/directory" should be replaced with the directory you want the file to be downloaded to on the users 3DS (e.g. "/cias"). The directory should always start with a slash and end without a slash.

**EXAMPLE:**
```
{
"Super Anthem 3D - Redmond":["https://drive.google.com/uc?export=download&id=1IrBWvuzu9lG0NqaTOKKsHVch2JJCFxTt","Redmond.mp3","/DCIM"],
"Super Anthem 3D - Washington":["https://drive.google.com/uc?export=download&id=15o5vDlao0ZqPsmNjQnqlJ1ADaidO8x72","Washington.mp3","/DCIM"],
"Portal 2 for 3DS":["https://drive.google.com/uc?export=download&id=1zr99VsVybvAacDvqwz2ij4B2fhassxrp","Portal 2.cia","/cias"]
}
```

Once the storefront.json file is filled out, go back to google drive and select "New>File Upload" and upload the storefront.json file. Once it's done uploading, get a link to the file with the instructions below. Next, go to [this](https://tinyurl.com/) site and put in that link. The site will ask for an alias. This will be the name of your storefront. Once that tinyurl link is created, open 3DS Storefront and press X once the current storefront loads. Then, you can type in the name of your new storefront. You can also share name of your storefront name with other poeple and they can use it.

### LINKS
To get a link to a file for your storefront.json, right-click on the file in google drive and select "Share>Copy Link". THIS LINK WILL NOT WORK. Instead, put that link into [this](https://sites.google.com/site/gdocs2direct/) site. The link you get back is the one you must use in the storefront.json
