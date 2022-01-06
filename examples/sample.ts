

// @filename: maths.ts
export var pi = 3.14;
export let squareTwo = 1.41;
export const phi = 1.61;

interface Link {
  description?: string;
  id?: number;
  url: string;
}

interface TranslatedLink extends Link {
  language: string;
}

const link1: TranslatedLink = {
  description:
    "TypeScript tutorial for beginners is a tutorial for all the JavaScript developers ...",
  id: 1,
  url: "www.valentinog.com/typescript/",
  language: "en"
};

export class RandomNumberGenerator {}
 
export function absolute(num: number) {
  if (num < 0) return num * -1;
  return num;
}

// Parameter type annotation
function greet(name: string) {
  console.log("Hello, " + name.toUpperCase() + "!!");
}
