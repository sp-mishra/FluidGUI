<template>
  <div>
    <div>
      <label :class="computedStyle">{{ text }}</label>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    text: {
      type: String,
      default: ''
    },
    size: {
      type: String,
      default: 'regular'
    },
    style: {
      type: String,
      default: 'normal'
    },
    color: {
      type: String,
      default: 'black'
    }
  },
  data() {
    return {
      userText: '',
      textSize: this.size,
      textStyle: this.style,
      textColor: this.color,
      textSizes: ['h1', 'h2', 'h3', 'h4', 'regular'],
      textStyles: ['normal', 'bold', 'italic'],
      textColors: ['black', 'red', 'blue', 'green', 'yellow']
    }
  },
  computed: {
    computedClass() {
      let classes = [];
      if (this.textSize) {
        classes.push(this.textSize);
      }
      if (this.textStyle) {
        classes.push(this.textStyle);
      }
      return classes.join(' ');
    }
  }
}
</script>

<style>
.h1 { font-size: 2em; }
.h2 { font-size: 1.5em; }
.h3 { font-size: 1.17em; }
.h4 { font-size: 1em; }
.h5 { font-size: 0.83em; }
.h6 { font-size: 0.67em; }
.small { font-size: 0.8em; }
.medium { font-size: 1.2em; }
.large { font-size: 1.5em; }
.regular { font-size: 1em; }
.bold { font-weight: bold; }
.italic { font-style: italic; }
.underline { text-decoration: underline; }
.strikethrough { text-decoration: line-through; }
.uppercase { text-transform: uppercase; }
.lowercase { text-transform: lowercase; }
.capitalize { text-transform: capitalize; }
.overline { text-decoration: overline; }
.smallcaps { font-variant: small-caps; }
.superscript { vertical-align: super; font-size: smaller; }
.subscript { vertical-align: sub; font-size: smaller; }
.black { color: black; }
.white { color: white; }
.red { color: red; }
.green { color: green; }
.blue { color: blue; }
.yellow { color: yellow; }
.orange { color: orange; }
.purple { color: purple; }
.pink { color: pink; }
.brown { color: brown; }
.gray { color: gray; }
.cyan { color: cyan; }
.magenta { color: magenta; }
.lime { color: lime; }
.maroon { color: maroon; }
.navy { color: navy; }
.olive { color: olive; }
.teal { color: teal; }
.violet { color: violet; }
.indigo { color: indigo; }
</style>