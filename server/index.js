const express = require('express')
const path = require('path')
const { Pool } = require('pg')

const PORT = process.env.PORT || 5000

console.log(process.env.DATABASE_URL)

const pool = new Pool({
    connectionString: process.env.DATABASE_URL,
    ssl: {
      rejectUnauthorized: false
    }
  });
  
pool.connect();

pool.on('error', (err, client) => {
    console.error('Unexpected error on idle client', err)
})

async function getScriptlet(req, res) {
    const { workshopId, userName } = req.params
    console.log("getScriptlet request", workshopId, userName)
    const client = await pool.connect()
    try {

        const { rows } = await client.query(
            `select scriptlet from code_canvas_entry where id = 
            (select max (id) from code_canvas_entry where workshop_id = $1 and nickname = $2);`, [workshopId, userName])

        if (rows.length == 0) {
            const scriptlet = 'x = t'    
            const { result } = await client.query(`insert into code_canvas_entry (workshop_id, nickname, scriptlet) 
            values ($1, $2, $3)`, [workshopId, userName, scriptlet]);
            res.send({scriptlet})
        } else {
            res.send(rows[0].scriptlet)
        }
    } catch (error) {
        console.log("login error", workshopId, userName, error)    
        res.status(500).send("login error")
    }
    finally {
        client.release()
    }
}

async function saveScriptlet(req, res) {
    const { workshopId, userName } = req.params
    const scriptlet = req.body.toString('utf8')
    console.log("saveScriptlet request", workshopId, userName, scriptlet.split(/[^- \t;+.,\(\)a-zA-Z=0-9]/).join("#").substring(0, 32))
    const client = await pool.connect()
  
    try {
        await client.query(`insert into code_canvas_entry (workshop_id, nickname, scriptlet) 
        values ($1, $2, $3)`, [workshopId, userName, scriptlet]);
        res.send({result: 'ok'})

    } catch (error) {
        console.log("save error", workshopId, userName, error)    
        res.status(500).send("save error")
    }
    finally {
        client.release()
    }
}

console.log("Setting up express")

express()
  .use(express.raw({type: "*/*"}))
  // |/workshoptest|/workshop/:workshopId([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})
  .use('/:a(workshop)?/:b([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})?/:c([^/]+)?/', express.static(path.join(__dirname, 'public')))
  //.use('/logintest/', express.static(path.join(__dirname, 'public')))
  .get('/api/0/:workshopId([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})/:userName/', getScriptlet)
  .post('/api/0/:workshopId([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})/:userName/', saveScriptlet)
  .listen(PORT, () => console.log(`Listening on ${ PORT }`))
