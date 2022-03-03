For now, I found a WorkAround if anyone is Interested,     USING TextMate-Grammer that VSCODE has Support for:
>Its Pretty much HardCoded based on What Characters are what..... Not really related to Fonts having the Italic Variations or not.... But at least it can FIX THE Emojis 😉





<details>
<summary>package.json    (in an extension)</summary>

```json
//package.json

"contributes": {
    "themes": [
        {
            "doc": "EXAMPLE THEME",
            "label": "Eva Dark",
            "uiTheme": "vs-dark",
            "path": "./themes/Eva-Dark.json"
        }
    ],
    "grammars": [
        {
            "doc": "as this is package.json   we have syntaxes folder which has another json",
            "path": "./syntaxes/test.tmGrammer.json",

            "doc": "should be matching to    'scopeName' that you have in test.tmGrammer.json",
            "scopeName": "NonASCII.injection.cpp",

            "doc":  "We want to INJECT, not OverWrite the Existing TEXT-MATE Scopes",
            "injectTo": ["source.cpp"]
        }
    ]
},
```
</details>







<details>
<summary>related TextMateGrammer.json</summary>

```json
// ./syntaxes/test.tmGrammer.json
// I know json doesn't support DOC-STRINGS

// Well, why not just highlight nonASCII chars       U can (non-)highlight your Main Language too if Unicode has that support
{
  "scopeName": "NonASCII.injection.cpp",
  "injectionSelector": [
    "L:comment.block.documentation.cpp",
    "L:comment.line.double-slash.cpp"
  ],
  "patterns": [
    {
      "include": "#Non-ASCII-Chars"
    }
  ],
  "repository": {
    "Non-ASCII-Chars": {
      "doc": "Note the double BACKSLASHES ;)",
      "match": "\\p{^ASCII}",
      "name": "comment.non-ascii.cpp"
   }
}
```
</details>




## NOTES
- A REGEX Example: https://github.com/microsoft/vscode-textmate/blob/main/test-cases/first-mate/fixtures/javascript-regex.json
[This is where I Got a LIVESAVING Sudden thought]
EXPLANATION:
    - I was using like    "\p{^ASCII}"     cz apple's textmate documentation only wanted that
    - But json actually is different,      json needs DOUBLE BACKSLASHES
    - SO LIFE-SAVING!!!!! (like TIME-TRAVEL!!!!)
</br>

- HOW DO YOU DOU     !this-logic      (Negative/Reverse/Not)?
    - https://superuser.com/questions/477463/is-it-possible-to-use-not-in-a-regular-expression-in-textmate

    - Or just mainly use    ^    (yes this Character)    whenever you can    [In most places you can & Should]
</br>

- These are the TEXTMATE Grammatical Options: https://macromates.com/manual/en/language_grammars

- **Then for any theme that makes COMMENTS Italic     you can use the scope that you just created, in my case that would be**  `comment.non-ascii.cpp`

- Well, you can choose any of the 'Character Property [3.]'    from     [TextMate Expressions](https://macromates.com/manual/en/regular_expressions)

- Here is the Semantic Guide part where you should start: https://code.visualstudio.com/api/language-extensions/syntax-highlight-guide#injection-grammars