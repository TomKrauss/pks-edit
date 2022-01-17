# Markdown Support in PKS-Edit

## Purpose

PKS-Edit supports Wysiwyg mode display of markdown text primarily for the purpose of providing documentation of PKS Edit itself.
Support is currently limited - not all markdown features are supported. In particular markdown text may only be displayed and not
be edited. One may browse markdown links, but that is all, which is possible. To edit markdown text, one need to change back to
ascii rendering mode.

## General Limitations

- Inline HTML is currently not supported (except using \<br> to force a line break in a paragraph)
- Only some extended markdown features are supported (tables). Other things like emoticons etc... are not supported.

## Supported Block Level Elements

### Heading Level 3

#### Heading Level 4

##### Heading Level 5

###### Heading Level 6

These are two text paragraphs. The 1st paragraph contains a <br> line break.

### Formatting and Links

Here is the second paragraph. It contains a long wrapped text. Note, that *emphasis* and __boldness__ or a ***combination*** are supported as well as a `code` 
type formatting of text. The markdown also may contain [links - e.g to the grammar](manual/grammar.md). One may also use ~~strikethrough~~ text.

Using formatting chars such as \_ in the middle_of_a_word eliminates the need to escape them. Using \_ at the border of a _word_ will have the word be stand out in _italic_.

You may also ==highlight== important places in the text.

[Auto-links](https://github.github.com/gfm/#autolinks) are supported to some extent using the following syntax `\<https://www.google.de>`: <www.google.de>.

[In-document links to anchors](#images "this brings you to the images") are supported as well.

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

Here is an indented code block:

	int i = 12;
	System.out.println(i);

## Images

Images are supported by PKS-Edit as well. The following samples show the supported image formats (`.bpm`, `.png`, `.jpg`, `.gif`).<br> 
Note, that the special syntax for defining width and height of the image as rendered as in the following code is supported `[The famous cat]\(cat.bmp =250x150)`.

### Limitations

Only local images are supported (no http support).

#### Some Examples


![The famous cat](cat.bmp =250x)

![Roll the dice](dice.png =250x)

![Balloons](balloons.jpg =250x)

![Flying Bird](bird.gif =250x)


## Tables

PKS-Edit supports the markdown extension tables. 

### Limitations

**Note**, that tables in PKS-Edit currently have quite some _limitations_:

- alignment in combination with formatting is not supported (formatted text will never be displayed aligned)
- table layout is performed rather heuristically and has lots of limitations
  
| Tables   |      Are      |  Cool |
|----------|:-------------:|------:|
| col 1 is |  left-aligned | $1600 |
| col 2 is |    centered   |   $12 |
| col 3 is | right-aligned |    $1 |
	
### More Table fun

| Long Content  | Formatted Content |
|----------|-------------|
| This is a very long content for one table cell. This is a very long content for one table cell. This is a very long content for one table cell. This is a very long content for one table cell... | This cell contains _formatted_ contents. |
| Some other `formatted` stuff, *italic*, **bold**, ~~strike through~~ | Cell with [link](manual/grammar.md) |

### Tables with padded Columns

Table rows must not have the same number of columns as the header and delimiter lines. If there are a number of cells fewer than the number of cells in the header row, empty cells are inserted. If there are greater, the excess is ignored.

This is an example which demonstrates the behavior:

```
Column1   | Column2  |  Column3  |
----------|-------------|------
Content11 |  Content12 | 
Content21 | Content22 | Content23 | Excess |
```
Column1   | Column2  |  Column3  |
----------|-------------|------
Content11 |  Content12 | 
Content21 | Content22 | Content23 | Excess |




