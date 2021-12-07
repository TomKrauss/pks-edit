
# Markdown Support in PKS-Edit

## Purpose

PKS-Edit supports Wysiwyg mode display of markdown text primarily for the purpose of providing documentation of PKS Edit itself.
Support is currently limited - not all markdown features are supported. In particular markdown text may only be displayed and not
be edited. One may browse markdown links, but that is all, which is possible. To edit markdown text, one need to change back to
ascii rendering mode.

## Limitations

- Inline HTML is currently not supported (except using \<br> to force a line break in a paragraph)
- No extended markdown features such as tables or emoticons are supported.

## Markdown samples

### Heading Level 3

#### Heading Level 4

##### Heading Level 5

###### Heading Level 6

These are two text paragraphs. The 1st paragraph contains a <br> line break.

### Links

Here is the second paragraph. It contains a long wrapped text. Note, that *emphasis* and __boldness__ or a ***combination*** are supported as well as a `code` 
type formatting of text. The markdown also may contain [links - e.g to the grammar](manual/grammar.md). One may also use ~~strikethrough~~ text.

[Auto-links](https://github.github.com/gfm/#autolinks) are supported to some extent using the following syntax `\<https://www.google.de>`: <www.google.de>.

[In-document links to anchors](#image-support) are supported as well.

### Block Quotes

> This is a block quote
> 
>> one level deeper
>
> Level 1
>
> Still on Level 1

### Unordered Lists

+ Here is an ordered list
  + with a nested list sub-element
    + three level deep list sub-element
+ Another top level-element

### Ordered Lists

1. Ordered lists
2. Unordered lists
   1. Nested list element
   2. Second in nested element 
3. etc...

### Tasklists

- [x] This is a checked task
- [ ] This is an unchecked task.

A horizontal rule follows.

---

Here is a fenced code block:

```
{
  "firstName": "John",
  "lastName": "Smith",
  "age": 25
}
```

## Image Support

Some more text at the end. And finally some images. Note, that the special syntax for defining width and height of the image as rendered as in the
following code is supported `[The famous cat]\(cat.bmp =250x150)`.

![The famous cat](cat.bmp =250x)

![Roll the dice](dice.png =250x)

![Balloons](balloons.jpg =250x)

![Flying Bird](bird.gif =250x)

Note, that image support is currently also rather limited: only local images are supported.

