# UID vs EUID

In this topic we are going to see the difference between uid and euid. We will also see a special attribute called setuid which will be discussed in the later part of the blog.

## UID
User ID, also called the real user ID. This corresponds to the real user ID of the user executing the program/process. This can only be set by super user and is usually set when the user logs in. Non-super user cannot change the user ID. 

## EUID
Effective User ID. This corresponds to the user ID the program/process is being executed as. The effective UID can be either set to read UID or set-UID. I will talk about the set-UID later in the article.

I know how confusing it can get. Lets use examples to get this clear. 

