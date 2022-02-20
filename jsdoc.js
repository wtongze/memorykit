'use strict';

module.exports = {
  plugins: ['plugins/markdown'],
  source: {
    include: ['./lib'],
  },
  opts: {
    destination: './doc',
    readme: 'README.md',
  },
};
