module.exports = {
  content: ['./app/**/*.{ts,tsx,jsx,js}'],
  theme: {
    extend: {
      fontFamily: {
        game: ['"Press Start 2P"', 'cursive'],
      },
      colors: {
        primary: '#4D0B17',
      },
      screens: {
        /**
         * sometimes its nice to say: mobileOnly or padOnly: without needing to reset it with md:...
         */
        mobile: { max: '639px' },
        pad: { max: '767px' },
      },
    },
  },
  variants: {},
  plugins: [],
};
