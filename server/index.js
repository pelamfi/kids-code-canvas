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

async function login(req, res) {
    const { workshopId, userName } = req.params

    const { rows } = await client.query(
        `select scriptlet from code_canvas_entry where id = 
        (select max (id) from code_canvas_entry where workshop_id = $1, nickname = $2)`, [workshopId, userId])

    if (rows.length == 0) {
        const { result } = await client.query(`insert into code_canvas_entry (workshop_id, nickname, scriptlet) 
        values ($1, $2, 'x = t'`, [workshopId, userName]);
    }
        
    res.send(rows)
}

express()
  .use(express.static(path.join(__dirname, 'public')))
  .get('/', (req, res) => res.render('public/index'))
  .get(/\/worshop\/[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}/, (req, res) => res.render('public/index'))
  .get(/\/worshop\/[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}\/:username/, (req, res) => res.render('public/index'))
  .get('/api/0/login', login)
  .listen(PORT, () => console.log(`Listening on ${ PORT }`))

async function test() {
  const client = await pool.connect()
  try {
    const res = await client.query('SELECT table_schema,table_name FROM information_schema.tables;')
    console.log(JSON.stringify(res.rows, null, 0));
  } finally {
    client.release()
  }
}

   
/*
const workshopId = '8a7f0b49-f389-44c4-ab12-0010d7a94b6b';    
const { result } = await client.query(`insert into code_canvas_entry (workshop_id, nickname, scriptlet) 
    values ($1, $2, 'x = t'`, [workshopId, userName]);
*/

test()
.catch(console.error)