package main

import "fmt"
import (
    "bytes"
    "fmt"
    "regexp"
)

func main() {

    var a = "initial"
    fmt.Println(a)
	
	// This is a GO comment
    var b, c int = 1, 2
    fmt.Println(b, c)

    var d = true
    fmt.Println(d)

    var e int
    fmt.Println(e)

    f := "apple"
    fmt.Println(f)

    messages := make(chan string)

    go func() { messages <- "ping" }()

    msg := <-messages
    fmt.Println(msg)

}
