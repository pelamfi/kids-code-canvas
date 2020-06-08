# Project Kids-Code-Canvas

## Project status

So far there is nothing here. Just trying out various web technologies
and getting the feel for them.

## Prepare for building project

The npx is used to run bsb locally, but npx still needs to be installed globally
This however reduces chances of using wrong bsb versions.

```sh
npm install -g npx 
```

## Run Project

```sh
npm install
npm start
# in another tab
npm run webpack
```

After you see the webpack compilation succeed (the `npm run webpack` step), open up `build/index.html` (**no server needed!**). Then modify whichever `.re` file in `src` and refresh the page to see the changes.

**For more elaborate ReasonReact examples**, please see https://github.com/reasonml-community/reason-react-example

## Run Project with Server

To run with the webpack development server run 

    npm run server
    
and view in the browser at:

  * http://localhost:8000.
  
Running in this environment provides hot reloading and support for routing; just edit and save the file and the browser will automatically refresh.

Note that any hot reload on a route will fall back to the root (`/`), so `ReasonReact.Router.dangerouslyGetInitialUrl` will likely be needed alongside the `ReasonReact.Router.watchUrl` logic to handle routing correctly on hot reload refreshes or simply opening the app at a URL that is not the root.

To use a port other than 8000 set the `PORT` environment variable (`PORT=8080 npm run server`).

## Build for Production

```sh
npm run clean
npm run build
npm run webpack:production
```

This will replace the development artifact `build/Index.js` for an optimized version as well as copy `src/index.html` into `build/`. You can then deploy the contents of the `build` directory (`index.html` and `Index.js`).

If you make use of routing (via `ReasonReact.Router` or similar logic) ensure that server-side routing handles your routes or that 404's are directed back to `index.html` (which is how the dev server is set up).

**To enable dead code elimination**, change `bsconfig.json`'s `package-specs` `module` from `"commonjs"` to `"es6"`. Then re-run the above 2 commands. This will allow Webpack to remove unused code.

### Notes on bsconfig.json

  * Add bsc options and super errors: https://bucklescript.github.io/docs/en/build-configuration.html
  * Enable debuggable data structures: https://bucklescript.github.io/docs/en/better-data-structures-printing-debug-mode

## Run tests

    npx jest

## Notes on Heroku

Install heroku command line tool.

    heroku create pelam-kids-code-canvas --region eu
      Creating ⬢ pelam-kids-code-canvas... done, region is eu
      https://pelam-kids-code-canvas.herokuapp.com/ | https://git.heroku.com/pelam-kids-code-canvas.git


  https://coderwall.com/p/ssxp5q/heroku-deployment-without-the-app-being-at-the-repo-root-in-a-subfolder

  git remote add heroku https://git.heroku.com/pelam-kids-code-canvas.git

  git subtree push --prefix server heroku master


## Collection of scriptlets

    x = sin(t * 3) + t * 0.2;y = cos(t * 3)

    x = t * 0.1 + (cos(t*9)+cos(t*2)) * 0.5; y = (sin(t*9)+sin(t*2)) * 0.5; sininen = cos(t * 3); vihrea = 0.5; punainen = sin(t)    

    x = sin(t * 3) + t * 0.25;y = cos(t * 3)
    if (t % 2 < 1) {
      x = t * 0.1 + (cos(t*9)+cos(t*2)) * 0.5; y = (sin(t*9)+sin(t*2)) * 0.5; sininen = cos(t * 3); vihrea = 0.5; punainen = sin(t)
    }

    x = t * 1001;y = t * pi;punainen = t * 33;sininen = t;vihrea = t

    x = t * 1001.12;y = t * pi;punainen = t * 33;sininen = t;vihrea = t

    koko = 2
    punainen = 1
    sininen = 1
    vihreä = 1
    if (t > 0.1) punainen = 0
    if (t > 0.1) sininen = 1
    if (t > 0.1) vihreä = 0
    if (t > 0.1) koko = 0.12
    if (t > 0.1) x = t
    if (t > 3.7) x = -0.4
    if (t > 3.7) y = t

    x = t
    y = t
    punainen = t
    vihreä = t
    sininen = t

    y = random() * 100
    x = random() * 100
    vihreä = sin(t)
    koko = random() / 5


    x on t kertaa 2
    y on t kertaa 6
    punainen on aalto(t)
    koko on aalto(t) / 3
    sininen on aalto(t kertaa 1.4)
    läpinäkyvyys on aalto(t)